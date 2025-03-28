#include "vulkanRenderer.h"
#include "compute.h"
#include "computePushConstant.h"
#include "computeShader.h"
#include "dearImGui.h"
#include "defaultPushConstant.h"
#include "drawCall.h"
#include "forwardRenderPass.h"
#include "graphics.h"
#include "lighting.h"
#include "material.h"
#include "materialManager.h"
#include "mesh.h"
#include "meshManager.h"
#include "pipeline.h"
#include "presentRenderPass.h"
#include "renderPassManager.h"
#include "renderTexture2d.h"
#include "shaderProperties.h"
#include "shadowPushConstant.h"
#include "shadowRenderPass.h"
#include "spirvReflect.h"
#include "emberTime.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanCommand.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include "vulkanUtility.h"



namespace emberEngine
{
	// Constructor/Destructor:
	VulkanRenderer::VulkanRenderer()
	{
		// Command buffers:
		m_syncComputeCommands.reserve(VulkanContext::framesInFlight);
		m_shadowCommands.reserve(VulkanContext::framesInFlight);
		m_forwardCommands.reserve(VulkanContext::framesInFlight);
		m_postProcessComputeCommands.reserve(VulkanContext::framesInFlight);
		m_presentCommands.reserve(VulkanContext::framesInFlight);
		for (uint32_t i = 0; i < VulkanContext::framesInFlight; i++)
		{
			m_syncComputeCommands.emplace_back(VulkanContext::pLogicalDevice->GetGraphicsQueue());
			m_shadowCommands.emplace_back(VulkanContext::pLogicalDevice->GetGraphicsQueue());
			m_forwardCommands.emplace_back(VulkanContext::pLogicalDevice->GetGraphicsQueue());
			m_postProcessComputeCommands.emplace_back(VulkanContext::pLogicalDevice->GetGraphicsQueue());
			m_presentCommands.emplace_back(VulkanContext::pLogicalDevice->GetGraphicsQueue());
		}

		// Synchronization objects:
		CreateFences();
		CreateSemaphores();

		// Injection systems initilization:
		Compute::Init();
		Lighting::Init();
		Graphics::Init();
	}
	VulkanRenderer::~VulkanRenderer()
	{
		VKA(vkDeviceWaitIdle(VulkanContext::GetVkDevice()));
		Graphics::Clear();
		Lighting::Clear();
		Compute::Clear();
		DestroySemaphores();
		DestroyFences();
	}



	// Public methods:
	bool VulkanRenderer::RenderFrame()
	{
		// Resize Swapchain if needed:
		VkExtent2D windowExtent = VulkanContext::pWindow->GetExtent();
		VkExtent2D surfaceExtend = VulkanContext::pSurface->GetCurrentExtent();
		if (m_rebuildSwapchain || windowExtent.width != surfaceExtend.width || windowExtent.height != surfaceExtend.height)
		{
			m_rebuildSwapchain = false;
			VulkanContext::ResetFrameIndex();
			RebuildSwapchain();
		}

		// Wait for fence of previous frame with same frameIndex to finish:
		VKA(vkWaitForFences(VulkanContext::GetVkDevice(), 1, &m_fences[VulkanContext::frameIndex], VK_TRUE, UINT64_MAX));
		if (!AcquireImage())
			return 0;
		VKA(vkResetFences(VulkanContext::GetVkDevice(), 1, &m_fences[VulkanContext::frameIndex]));

		// Get draw calls:
		m_pDrawCalls = Graphics::GetSortedDrawCallPointers();

		// Record and submit frame:
		RecordComputeCommandBuffer();
		RecordShadowCommandBuffer();
		RecordForwardCommandBuffer();
		RecordPostProcessComputeCommandBuffer();
		RecordPresentCommandBuffer();
		SubmitCommandBuffers();

		// Reset engine data for next frame:
		Compute::ResetComputeCalls();
		Lighting::ResetLights();
		Graphics::ResetDrawAndComputeCalls();

		if (!PresentImage())
			return 0;

		return 1;
	}



	// Private methods:
	void VulkanRenderer::RebuildSwapchain()
	{
		// Wait for graphicsQueue to finish:
		VKA(vkQueueWaitIdle(VulkanContext::pLogicalDevice->GetGraphicsQueue().queue));

		// Recreate swapchain:
		std::unique_ptr<VulkanSwapchain> newSwapchain = std::make_unique<VulkanSwapchain>(VulkanContext::pLogicalDevice.get(), VulkanContext::pSurface.get(), VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VulkanContext::pSwapchain.get());
		VulkanContext::pSwapchain.swap(newSwapchain);

		// Recreate renderpasses:
		RenderPassManager::RecreateRenderPasses();

		// Recreate synchronization objects:
		DestroyFences();
		DestroySemaphores();
		CreateFences();
		CreateSemaphores();
	}
	bool VulkanRenderer::AcquireImage()
	{
		// Signal acquireSemaphore when done:
		VkResult result = vkAcquireNextImageKHR(VulkanContext::GetVkDevice(), VulkanContext::GetVkSwapchainKHR(), UINT64_MAX, m_acquireSemaphores[VulkanContext::frameIndex], VK_NULL_HANDLE, &m_imageIndex);

		// Resize if needed:
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || VulkanContext::pWindow->GetFramebufferResized())
		{
			VulkanContext::pWindow->SetFramebufferResized(false);
			m_rebuildSwapchain = true;
			return false;
		}
		else
		{
			VKA(result);
			return true;
		}
	}
	
	void VulkanRenderer::RecordComputeCommandBuffer()
	{
		vkResetCommandPool(VulkanContext::GetVkDevice(), m_syncComputeCommands[VulkanContext::frameIndex].GetVkCommandPool(), 0);
		VkCommandBuffer commandBuffer = m_syncComputeCommands[VulkanContext::frameIndex].GetVkCommandBuffer();

		// Begin command buffer:
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			ComputeShader* pComputeShader = nullptr;
			ComputeShader* pPreviousComputeShader = nullptr;
			VkPipeline pipeline = VK_NULL_HANDLE;
			VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
			ComputePushConstant pushConstant(Uint3::one, Time::GetTime(), Time::GetDeltaTime());

			for (ComputeCall* computeCall : Compute::GetComputeCallPointers())
			{
				// Compute call is a barrier:
				if (computeCall->pComputeShader == nullptr)
				{
					VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
					memoryBarrier.srcStageMask = PipelineStage::computeShader;
					memoryBarrier.dstStageMask = PipelineStage::computeShader;
					memoryBarrier.srcAccessMask = computeCall->srcAccessMask;
					memoryBarrier.dstAccessMask = computeCall->dstAccessMask;

					VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
					dependencyInfo.memoryBarrierCount = 1;
					dependencyInfo.pMemoryBarriers = &memoryBarrier;

					vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
				}
				// Compute call is a dispatch call:
				else
				{
					// Update shader specific data:
					computeCall->pShaderProperties->UpdateShaderData();

					// Change pipeline if compute shader has changed:
					pComputeShader = computeCall->pComputeShader;
					if (pPreviousComputeShader != pComputeShader)
					{
						pPreviousComputeShader = pComputeShader;
						pipeline = pComputeShader->GetPipeline()->GetVkPipeline();
						pipelineLayout = pComputeShader->GetPipeline()->GetVkPipelineLayout();
						pushConstant.threadCount = computeCall->threadCount;
						vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
						ComputePushConstant pushConstant(computeCall->threadCount, Time::GetTime(), Time::GetDeltaTime());
						vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstant), &pushConstant);
					}

					// Same compute shader but different thread Count => update push constants:
					if (pushConstant.threadCount != computeCall->threadCount)
					{
						pushConstant.threadCount = computeCall->threadCount;
						vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstant), &pushConstant);
					}

					Uint3 blockSize = pComputeShader->GetBlockSize();
					uint32_t groupCountX = (computeCall->threadCount.x + blockSize.x - 1) / blockSize.x;
					uint32_t groupCountY = (computeCall->threadCount.y + blockSize.y - 1) / blockSize.y;
					uint32_t groupCountZ = (computeCall->threadCount.z + blockSize.z - 1) / blockSize.z;

					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &computeCall->pShaderProperties->GetDescriptorSet(VulkanContext::frameIndex), 0, nullptr);
					vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
				}
			}

			// Release memory from compute shaders to vertex shaders:
			{
				VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
				memoryBarrier.srcStageMask = PipelineStage::computeShader;
				memoryBarrier.dstStageMask = PipelineStage::vertexShader;
				memoryBarrier.srcAccessMask = VK_ACCESS_2_SHADER_WRITE_BIT;
				memoryBarrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;

				VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
				dependencyInfo.memoryBarrierCount = 1;
				dependencyInfo.pMemoryBarriers = &memoryBarrier;

				vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
			}
		}
		VKA(vkEndCommandBuffer(commandBuffer));
	}
	void VulkanRenderer::RecordShadowCommandBuffer()
	{
		// Static parameters initialization:
		static Material* pShadowMaterial;
		static VkPipeline shadowPipeline;
		static VkPipelineLayout shadowPipelineLayout;
		if (pShadowMaterial == nullptr)
		{
			pShadowMaterial = MaterialManager::GetMaterial("shadowMaterial");
			shadowPipeline = pShadowMaterial->GetPipeline()->GetVkPipeline();
			shadowPipelineLayout = pShadowMaterial->GetPipeline()->GetVkPipelineLayout();
		}

		vkResetCommandPool(VulkanContext::GetVkDevice(), m_shadowCommands[VulkanContext::frameIndex].GetVkCommandPool(), 0);
		VkCommandBuffer commandBuffer = m_shadowCommands[VulkanContext::frameIndex].GetVkCommandBuffer();

		// Begin command buffer:
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			// Render pass info:
			VkClearValue clearValues = {};
			clearValues.depthStencil = { 1.0f, 0 };
			VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
			renderPassBeginInfo.renderPass = RenderPassManager::GetShadowRenderPass()->GetVkRenderPass();
			renderPassBeginInfo.framebuffer = RenderPassManager::GetShadowRenderPass()->GetFramebuffers()[0];
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent = VkExtent2D{ Lighting::shadowMapResolution, Lighting::shadowMapResolution };
			renderPassBeginInfo.clearValueCount = 1;
			renderPassBeginInfo.pClearValues = &clearValues;
			const VkDeviceSize offsets[1] = { 0 };

			// Begin render pass:
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			{
				// BestPractices validation layer warning occur when binding a pipeline with vertex input data, but not binding any vertex buffer.
				if (Lighting::GetDirectionalLightsCount() > 0 || Lighting::GetPositionalLightsCount() > 0)
				{
					const VkPipeline& shadowPipeline = pShadowMaterial->GetPipeline()->GetVkPipeline();
					const VkPipelineLayout& shadowPipelineLayout = pShadowMaterial->GetPipeline()->GetVkPipelineLayout();
					int shadowMapIndex = 0;
					vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowPipeline);

					// Update shader specific data:
					for (DrawCall* drawCall : *m_pDrawCalls)
						drawCall->pShadowShaderProperties->UpdateShaderData();

					// Directional Lights:
					std::array<Lighting::DirectionalLight, Lighting::maxDirectionalLights>& directionalLights = Lighting::GetDirectionalLights();
					for (int i = 0; i < Lighting::GetDirectionalLightsCount(); i++)
					{
						Lighting::DirectionalLight& light = directionalLights[i];
						for (DrawCall* drawCall : *m_pDrawCalls)
						{
							if (drawCall->castShadows == false)
								continue;

							Mesh* pMesh = drawCall->pMesh;

							ShadowPushConstant pushConstant(drawCall->instanceCount, shadowMapIndex, drawCall->localToWorldMatrix, light.worldToClipMatrix);
							vkCmdPushConstants(commandBuffer, shadowPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ShadowPushConstant), &pushConstant);

							vkCmdBindVertexBuffers(commandBuffer, 0, 1, &pMesh->GetVertexBuffer()->GetVkBuffer(), offsets);
							vkCmdBindIndexBuffer(commandBuffer, pMesh->GetIndexBuffer()->GetVkBuffer(), 0, Mesh::GetIndexType());

							vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowPipelineLayout, 0, 1, &drawCall->pShadowShaderProperties->GetDescriptorSet(VulkanContext::frameIndex), 0, nullptr);
							vkCmdDrawIndexed(commandBuffer, 3 * pMesh->GetTriangleCount(), math::Max(drawCall->instanceCount, (uint32_t)1), 0, 0, 0);
						}
						shadowMapIndex++;
					}

					// Positional Lights:
					std::array<Lighting::PositionalLight, Lighting::maxPositionalLights>& positionalLights = Lighting::GetPositionalLights();
					for (int i = 0; i < Lighting::GetPositionalLightsCount(); i++)
					{
						Lighting::PositionalLight& light = positionalLights[i];
						for (DrawCall* drawCall : *m_pDrawCalls)
						{
							if (drawCall->castShadows == false)
								continue;

							Mesh* pMesh = drawCall->pMesh;

							ShadowPushConstant pushConstant(drawCall->instanceCount, shadowMapIndex, drawCall->localToWorldMatrix, light.worldToClipMatrix);
							vkCmdPushConstants(commandBuffer, shadowPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ShadowPushConstant), &pushConstant);

							vkCmdBindVertexBuffers(commandBuffer, 0, 1, &pMesh->GetVertexBuffer()->GetVkBuffer(), offsets);
							vkCmdBindIndexBuffer(commandBuffer, pMesh->GetIndexBuffer()->GetVkBuffer(), 0, Mesh::GetIndexType());

							vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowPipelineLayout, 0, 1, &drawCall->pShadowShaderProperties->GetDescriptorSet(VulkanContext::frameIndex), 0, nullptr);
							vkCmdDrawIndexed(commandBuffer, 3 * pMesh->GetTriangleCount(), math::Max(drawCall->instanceCount, (uint32_t)1), 0, 0, 0);
						}
						shadowMapIndex++;
					}
				}
			}
			vkCmdEndRenderPass(commandBuffer);
		}
		VKA(vkEndCommandBuffer(commandBuffer));
	}
	void VulkanRenderer::RecordForwardCommandBuffer()
	{
		vkResetCommandPool(VulkanContext::GetVkDevice(), m_forwardCommands[VulkanContext::frameIndex].GetVkCommandPool(), 0);
		VkCommandBuffer commandBuffer = m_forwardCommands[VulkanContext::frameIndex].GetVkCommandBuffer();

		// Begin command buffer:
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			// Viewport and scissor:
			VkViewport viewport = {};
			viewport.width = RenderPassManager::GetForwardRenderPass()->GetRenderTexture()->GetWidth();
			viewport.height = RenderPassManager::GetForwardRenderPass()->GetRenderTexture()->GetHeight();
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			VkRect2D scissor = {};
			scissor.extent.width = viewport.width;
			scissor.extent.height = viewport.height;
			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

			// Render pass info:
			std::array<VkClearValue, 2> clearValues;
			clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
			clearValues[1].depthStencil = { 1.0f, 0 };
			VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
			renderPassBeginInfo.renderPass = RenderPassManager::GetForwardRenderPass()->GetVkRenderPass();
			renderPassBeginInfo.framebuffer = RenderPassManager::GetForwardRenderPass()->GetFramebuffers()[0];
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent.width = viewport.width;
			renderPassBeginInfo.renderArea.extent.height = viewport.height;
			renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassBeginInfo.pClearValues = clearValues.data();

			// Begin render pass:
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			{
				uint32_t bindingCount = 0;
				Mesh* pMesh = nullptr;
				Material* pMaterial = nullptr;
				Material* pPreviousMaterial = nullptr;
				VkPipeline pipeline = VK_NULL_HANDLE;
				VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
				DefaultPushConstant pushConstant(0, Time::GetTime(), Time::GetDeltaTime(), Lighting::GetDirectionalLightsCount(), Lighting::GetPositionalLightsCount(), Graphics::GetActiveCamera().position);

				// Normal draw calls:
				for (DrawCall* drawCall : *m_pDrawCalls)
				{
					pMesh = drawCall->pMesh;

					// Update shader specific data:
					drawCall->SetRenderMatrizes(Graphics::GetActiveCamera().viewMatrix, Graphics::GetActiveCamera().projectionMatrix);
					drawCall->SetLightData();
					drawCall->pShaderProperties->UpdateShaderData();

					// Change pipeline if material has changed:
					pMaterial = drawCall->pMaterial;
					if (pPreviousMaterial != pMaterial)
					{
						pPreviousMaterial = pMaterial;
						pipeline = pMaterial->GetPipeline()->GetVkPipeline();
						pipelineLayout = pMaterial->GetPipeline()->GetVkPipelineLayout();
						bindingCount = pMaterial->GetVertexInputDescriptions()->size;
						pushConstant.instanceCount = drawCall->instanceCount;
						vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
						vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(DefaultPushConstant), &pushConstant);
					}

					// Same material but different instance Count => update push constants:
					if (pushConstant.instanceCount != drawCall->instanceCount)
					{
						pushConstant.instanceCount = drawCall->instanceCount;
						vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(DefaultPushConstant), &pushConstant);
					}

					vkCmdBindVertexBuffers(commandBuffer, 0, bindingCount, pMaterial->GetMeshBuffers(pMesh), pMaterial->GetMeshOffsets(pMesh));
					vkCmdBindIndexBuffer(commandBuffer, pMesh->GetIndexBuffer()->GetVkBuffer(), 0, Mesh::GetIndexType());

					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &drawCall->pShaderProperties->GetDescriptorSet(VulkanContext::frameIndex), 0, nullptr);
					vkCmdDrawIndexed(commandBuffer, 3 * pMesh->GetTriangleCount(), math::Max(drawCall->instanceCount, (uint32_t)1), 0, 0, 0);
				}
			}
			vkCmdEndRenderPass(commandBuffer);

			// Release memory from vertex shaders to compute shaders:
			{
				VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
				memoryBarrier.srcStageMask = PipelineStage::vertexShader;
				memoryBarrier.dstStageMask = PipelineStage::computeShader;
				memoryBarrier.srcAccessMask = AccessMask::VertexShader::shaderRead;
				memoryBarrier.dstAccessMask = AccessMask::ComputeShader::shaderWrite;

				VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
				dependencyInfo.memoryBarrierCount = 1;
				dependencyInfo.pMemoryBarriers = &memoryBarrier;

				vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
			}
		}
		VKA(vkEndCommandBuffer(commandBuffer));

		// Ember::TODO: remove layout tracking?
		// Manually change cpu side image layout to general as this is the automatic final layout of the forward renderpass.
		RenderPassManager::GetForwardRenderPass()->GetRenderTexture()->GetVmaImage()->SetLayout(VK_IMAGE_LAYOUT_GENERAL);
	}
	void VulkanRenderer::RecordPostProcessComputeCommandBuffer()
	{
		vkResetCommandPool(VulkanContext::GetVkDevice(), m_postProcessComputeCommands[VulkanContext::frameIndex].GetVkCommandPool(), 0);
		VkCommandBuffer commandBuffer = m_postProcessComputeCommands[VulkanContext::frameIndex].GetVkCommandBuffer();

		// Begin command buffer:
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			ComputeShader* pComputeShader = nullptr;
			ComputeShader* pPreviousComputeShader = nullptr;
			VkPipeline pipeline = VK_NULL_HANDLE;
			VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
			ComputePushConstant pushConstant(Uint3::one, Time::GetTime(), Time::GetDeltaTime());

			uint32_t callIndex = 0;
			for (ComputeCall& computeCall : Graphics::GetPostProcessComputeCalls())
			{
				// Update shader specific data:
				if (callIndex % 2 == 0)
				{
					computeCall.pShaderProperties->SetTexture2d("inputImage", RenderPassManager::GetForwardRenderPass()->GetRenderTexture());
					computeCall.pShaderProperties->SetTexture2d("outputImage", RenderPassManager::GetForwardRenderPass()->GetSecondaryRenderTexture());
				}
				else
				{
					computeCall.pShaderProperties->SetTexture2d("inputImage", RenderPassManager::GetForwardRenderPass()->GetSecondaryRenderTexture());
					computeCall.pShaderProperties->SetTexture2d("outputImage", RenderPassManager::GetForwardRenderPass()->GetRenderTexture());
				}
				callIndex++;
				computeCall.pShaderProperties->UpdateShaderData();

				// Change pipeline if compute shader has changed:
				pComputeShader = computeCall.pComputeShader;
				if (pPreviousComputeShader != pComputeShader)
				{
					pPreviousComputeShader = pComputeShader;
					pipeline = pComputeShader->GetPipeline()->GetVkPipeline();
					pipelineLayout = pComputeShader->GetPipeline()->GetVkPipelineLayout();
					pushConstant.threadCount = computeCall.threadCount;
					vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
					ComputePushConstant pushConstant(computeCall.threadCount, Time::GetTime(), Time::GetDeltaTime());
					vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstant), &pushConstant);
				}

				// Same compute shader but different thread Count => update push constants:
				if (pushConstant.threadCount != computeCall.threadCount)
				{
					pushConstant.threadCount = computeCall.threadCount;
					vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstant), &pushConstant);
				}

				Uint3 blockSize = pComputeShader->GetBlockSize();
				uint32_t groupCountX = (computeCall.threadCount.x + blockSize.x - 1) / blockSize.x;
				uint32_t groupCountY = (computeCall.threadCount.y + blockSize.y - 1) / blockSize.y;
				uint32_t groupCountZ = (computeCall.threadCount.z + blockSize.z - 1) / blockSize.z;

				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &computeCall.pShaderProperties->GetDescriptorSet(VulkanContext::frameIndex), 0, nullptr);
				vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);

				// All effects access the same inputImage and outputImage.
				// Thus add memory barrier between every effect:
				{
					VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
					memoryBarrier.srcStageMask = PipelineStage::computeShader;
					memoryBarrier.dstStageMask = PipelineStage::computeShader;
					memoryBarrier.srcAccessMask = AccessMask::ComputeShader::shaderWrite;
					memoryBarrier.dstAccessMask = AccessMask::ComputeShader::shaderRead;

					VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
					dependencyInfo.memoryBarrierCount = 1;
					dependencyInfo.pMemoryBarriers = &memoryBarrier;

					vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
				}
			}

			// Transition renderTexture image to shader read only optimal:
			{
				VkImageLayout newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				VkPipelineStageFlags2 srcStage = PipelineStage::computeShader;
				VkPipelineStageFlags2 dstStage = PipelineStage::fragmentShader;
				VkAccessFlags2 srcAccessMask = AccessMask::ComputeShader::shaderWrite;
				VkAccessFlags2 dstAccessMask = AccessMask::FragmentShader::shaderRead;
				RenderPassManager::GetForwardRenderPass()->GetRenderTexture()->GetVmaImage()->TransitionLayout(commandBuffer, newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
			}
		}
		VKA(vkEndCommandBuffer(commandBuffer));
	}
	void VulkanRenderer::RecordPresentCommandBuffer()
	{
		// Static parameters initialization:
		static Mesh* pMesh;
		static Material* pMaterial;
		static ShaderProperties* pShaderProperties;
		static VkPipeline pipeline = VK_NULL_HANDLE;
		static VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
		static uint32_t bindingCount = 0;
		if (pMesh == nullptr)
		{
			pMesh = MeshManager::GetMesh("fullScreenRenderQuad");
			pMaterial = MaterialManager::GetMaterial("presentMaterial");
			pShaderProperties = new ShaderProperties((Shader*)pMaterial);
			pipeline = pMaterial->GetPipeline()->GetVkPipeline();
			pipelineLayout = pMaterial->GetPipeline()->GetVkPipelineLayout();
			bindingCount = pMaterial->GetVertexInputDescriptions()->size;
		}

		vkResetCommandPool(VulkanContext::GetVkDevice(), m_presentCommands[VulkanContext::frameIndex].GetVkCommandPool(), 0);
		VkCommandBuffer commandBuffer = m_presentCommands[VulkanContext::frameIndex].GetVkCommandBuffer();

		// Begin command buffer:
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			// Viewport and scissor:
			VkViewport viewport = {};
			viewport.width = (float)VulkanContext::pSurface->GetCurrentExtent().width;
			viewport.height = (float)VulkanContext::pSurface->GetCurrentExtent().height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			VkRect2D scissor = {};
			scissor.extent.width = viewport.width;
			scissor.extent.height = viewport.height;
			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

			// Render pass info:
			PresentRenderPass* presentRenderPass = RenderPassManager::GetPresentRenderPass();
			VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
			renderPassBeginInfo.renderPass = presentRenderPass->GetVkRenderPass();
			renderPassBeginInfo.framebuffer = presentRenderPass->GetFramebuffers()[m_imageIndex];
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent = VulkanContext::pSurface->GetCurrentExtent();

			// Begin render pass:
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			{
				// Update shader specific data:
				pShaderProperties->SetTexture2d("renderTexture", RenderPassManager::GetForwardRenderPass()->GetRenderTexture());
				pShaderProperties->UpdateShaderData();

				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

				vkCmdBindVertexBuffers(commandBuffer, 0, bindingCount, pMaterial->GetMeshBuffers(pMesh), pMaterial->GetMeshOffsets(pMesh));
				vkCmdBindIndexBuffer(commandBuffer, pMesh->GetIndexBuffer()->GetVkBuffer(), 0, Mesh::GetIndexType());

				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &pShaderProperties->GetDescriptorSet(VulkanContext::frameIndex), 0, nullptr);
				vkCmdDrawIndexed(commandBuffer, 3 * pMesh->GetTriangleCount(), 1, 0, 0, 0);
			}
			DearImGui::Render(commandBuffer);
			vkCmdEndRenderPass(commandBuffer);
		}
		VKA(vkEndCommandBuffer(commandBuffer));

		// Ember::TODO: remove layout tracking?
		// Manually change cpu side image layout to general as this is the automatic final layout of the forward renderpass.
		//RenderPassManager::GetForwardRenderPass()->GetRenderTexture()->GetVmaImage()->SetLayout(VK_IMAGE_LAYOUT_GENERAL);
	}

	void VulkanRenderer::SubmitCommandBuffers()
	{
		// ToTry:
		// Compute shaders do not need to wait for aquire semaphore. Instead make them only wait for previous compute to finish.
		// Shadow shaders then wait for aquire and computeToShadow semaphore.

		// Synchronized pre-graphics compute submission:
		{
			VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;							// wait at compute stage until semaphore is signaled
			VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &m_acquireSemaphores[VulkanContext::frameIndex];					// wait for acquire semaphore
			submitInfo.pWaitDstStageMask = &waitStage;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_syncComputeCommands[VulkanContext::frameIndex].GetVkCommandBuffer();
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &m_syncComputeToShadowSemaphores[VulkanContext::frameIndex];		// signal syncComputeToShadow semaphore when done
			VKA(vkQueueSubmit(VulkanContext::pLogicalDevice->GetGraphicsQueue().queue, 1, &submitInfo, nullptr)); // no fence needed (sync by semaphore)
		}

		// Shadow render pass submission:
		{
			VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;					// wait at depth and stencil test stage until semaphore is signaled
			VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &m_syncComputeToShadowSemaphores[VulkanContext::frameIndex];		// wait for synchronized compute commands to finish
			submitInfo.pWaitDstStageMask = &waitStage;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_shadowCommands[VulkanContext::frameIndex].GetVkCommandBuffer();
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &m_shadowToForwardSemaphores[VulkanContext::frameIndex];			// signal shadowToForward semaphore when done
			VKA(vkQueueSubmit(VulkanContext::pLogicalDevice->GetGraphicsQueue().queue, 1, &submitInfo, nullptr)); // no fence needed (sync by semaphore)
		}

		// Forward render pass submission:
		{
			VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;							// wait at fragment shader stage until semaphore is signaled
			VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &m_shadowToForwardSemaphores[VulkanContext::frameIndex];			// wait for shadowToForward semaphore
			submitInfo.pWaitDstStageMask = &waitStage;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_forwardCommands[VulkanContext::frameIndex].GetVkCommandBuffer();
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &m_forwardToPostProcessSemaphores[VulkanContext::frameIndex];	// signal forwardToPostProcess semaphore when done
			VKA(vkQueueSubmit(VulkanContext::pLogicalDevice->GetGraphicsQueue().queue, 1, &submitInfo, nullptr)); // no fence needed (sync by semaphore)
		}

		// Post processing compute submission:
		{
			VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;							// wait at compute stage until semaphore is signaled
			VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &m_forwardToPostProcessSemaphores[VulkanContext::frameIndex];	// wait for forwardToPostProcess semaphore
			submitInfo.pWaitDstStageMask = &waitStage;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_postProcessComputeCommands[VulkanContext::frameIndex].GetVkCommandBuffer();
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &m_postProcessToPresentSemaphores[VulkanContext::frameIndex];	// signal postProcessToPresent semaphore when done
			VKA(vkQueueSubmit(VulkanContext::pLogicalDevice->GetGraphicsQueue().queue, 1, &submitInfo, nullptr));  // no fence needed (sync by semaphore)
		}

		// Present render pass submission:
		{
			VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;							// wait at fragment shader stage until semaphore is signaled
			VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &m_postProcessToPresentSemaphores[VulkanContext::frameIndex];	// wait for m_postProcessToPresent semaphore
			submitInfo.pWaitDstStageMask = &waitStage;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_presentCommands[VulkanContext::frameIndex].GetVkCommandBuffer();
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &m_releaseSemaphores[VulkanContext::frameIndex];					// signal release semaphor when done
			VKA(vkQueueSubmit(VulkanContext::pLogicalDevice->GetGraphicsQueue().queue, 1, &submitInfo, m_fences[VulkanContext::frameIndex]));  // signal fence when done
		}
	}
	bool VulkanRenderer::PresentImage()
	{
		VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_releaseSemaphores[VulkanContext::frameIndex];	// wait for releaseSemaphor
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &VulkanContext::GetVkSwapchainKHR();
		presentInfo.pImageIndices = &m_imageIndex;

		VkResult result = vkQueuePresentKHR(VulkanContext::pLogicalDevice->GetPresentQueue().queue, &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			m_rebuildSwapchain = true;
			return false;
		}
		else
		{
			VKA(result);
			return true;
		}
	}

	void VulkanRenderer::CreateFences()
	{
		VkFenceCreateInfo createInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
		createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;	// Fence is created in the signaled state to prevent the first wait from blocking.

		m_fences.resize(VulkanContext::framesInFlight);
		for (uint32_t i = 0; i < VulkanContext::framesInFlight; i++)
			VKA(vkCreateFence(VulkanContext::GetVkDevice(), &createInfo, nullptr, &m_fences[i]));
	}
	void VulkanRenderer::CreateSemaphores()
	{
		VkSemaphoreCreateInfo createInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

		m_acquireSemaphores.resize(VulkanContext::framesInFlight);
		m_syncComputeToShadowSemaphores.resize(VulkanContext::framesInFlight);
		m_shadowToForwardSemaphores.resize(VulkanContext::framesInFlight);
		m_forwardToPostProcessSemaphores.resize(VulkanContext::framesInFlight);
		m_postProcessToPresentSemaphores.resize(VulkanContext::framesInFlight);
		m_releaseSemaphores.resize(VulkanContext::framesInFlight);
		for (uint32_t i = 0; i < VulkanContext::framesInFlight; i++)
		{
			VKA(vkCreateSemaphore(VulkanContext::GetVkDevice(), &createInfo, nullptr, &m_acquireSemaphores[i]));
			VKA(vkCreateSemaphore(VulkanContext::GetVkDevice(), &createInfo, nullptr, &m_syncComputeToShadowSemaphores[i]));
			VKA(vkCreateSemaphore(VulkanContext::GetVkDevice(), &createInfo, nullptr, &m_shadowToForwardSemaphores[i]));
			VKA(vkCreateSemaphore(VulkanContext::GetVkDevice(), &createInfo, nullptr, &m_forwardToPostProcessSemaphores[i]));
			VKA(vkCreateSemaphore(VulkanContext::GetVkDevice(), &createInfo, nullptr, &m_postProcessToPresentSemaphores[i]));
			VKA(vkCreateSemaphore(VulkanContext::GetVkDevice(), &createInfo, nullptr, &m_releaseSemaphores[i]));
		}
	}
	void VulkanRenderer::DestroyFences()
	{
		for (uint32_t i = 0; i < VulkanContext::framesInFlight; i++)
			vkDestroyFence(VulkanContext::GetVkDevice(), m_fences[i], nullptr);
		m_fences.clear();
	}
	void VulkanRenderer::DestroySemaphores()
	{
		for (uint32_t i = 0; i < VulkanContext::framesInFlight; i++)
		{
			vkDestroySemaphore(VulkanContext::GetVkDevice(), m_acquireSemaphores[i], nullptr);
			vkDestroySemaphore(VulkanContext::GetVkDevice(), m_syncComputeToShadowSemaphores[i], nullptr);
			vkDestroySemaphore(VulkanContext::GetVkDevice(), m_shadowToForwardSemaphores[i], nullptr);
			vkDestroySemaphore(VulkanContext::GetVkDevice(), m_forwardToPostProcessSemaphores[i], nullptr);
			vkDestroySemaphore(VulkanContext::GetVkDevice(), m_postProcessToPresentSemaphores[i], nullptr);
			vkDestroySemaphore(VulkanContext::GetVkDevice(), m_releaseSemaphores[i], nullptr);
		}
		m_acquireSemaphores.clear();
		m_syncComputeToShadowSemaphores.clear();
		m_shadowToForwardSemaphores.clear();
		m_forwardToPostProcessSemaphores.clear();
		m_postProcessToPresentSemaphores.clear();
		m_releaseSemaphores.clear();
	}
}