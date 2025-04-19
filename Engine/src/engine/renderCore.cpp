#include "renderCore.h"
#include "compute.h"
#include "computeShader.h"
#include "dearImGui.h"
#include "drawCall.h"
#include "emberTime.h"
#include "graphics.h"
#include "indexBuffer.h"
#include "lighting.h"
#include "material.h"
#include "materialManager.h"
#include "mesh.h"
#include "meshManager.h"
#include "renderPassManager.h"
#include "renderTexture2d.h"
#include "shaderProperties.h"
#include "spirvReflect.h"
#include "vertexBuffer.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanAccessMasks.h"
#include "vulkanCommandPool.h"
#include "vulkanComputePushConstant.h"
#include "vulkanContext.h"
#include "vulkanDefaultPushConstant.h"
#include "vulkanForwardRenderPass.h"
#include "vulkanMacros.h"
#include "vulkanPipeline.h"
#include "vulkanPipelineStages.h"
#include "vulkanPresentRenderPass.h"
#include "vulkanShadowPushConstant.h"
#include "vulkanShadowRenderPass.h"



namespace emberEngine
{
	using namespace vulkanBackend;



	// Constructor/Destructor:
	RenderCore::RenderCore()
	{
		// Command pools:
		m_commandPools.reserve(Context::framesInFlight);
		for (uint32_t i = 0; i < Context::framesInFlight; i++)
			m_commandPools.emplace_back(RenderStage::stageCount, Context::pLogicalDevice->GetGraphicsQueue());
		 
		// Command buffers:
		//m_preRenderComputeCommands.reserve(Context::framesInFlight);
		//m_shadowCommands.reserve(Context::framesInFlight);
		//m_forwardCommands.reserve(Context::framesInFlight);
		//m_postRenderComputeCommands.reserve(Context::framesInFlight);
		//m_presentCommands.reserve(Context::framesInFlight);
		//for (uint32_t i = 0; i < Context::framesInFlight; i++)
		//{
		//	m_preRenderComputeCommands.emplace_back(Context::pLogicalDevice->GetGraphicsQueue());
		//	m_shadowCommands.emplace_back(Context::pLogicalDevice->GetGraphicsQueue());
		//	m_forwardCommands.emplace_back(Context::pLogicalDevice->GetGraphicsQueue());
		//	m_postRenderComputeCommands.emplace_back(Context::pLogicalDevice->GetGraphicsQueue());
		//	m_presentCommands.emplace_back(Context::pLogicalDevice->GetGraphicsQueue());
		//}

		// Synchronization objects:
		CreateFences();
		CreateSemaphores();
	}
	RenderCore::~RenderCore()
	{
		VKA(vkDeviceWaitIdle(Context::GetVkDevice()));
		DestroySemaphores();
		DestroyFences();
	}



	// Public methods:
	bool RenderCore::RenderFrame()
	{
		// Resize Swapchain if needed:
		VkExtent2D windowExtent = Context::pWindow->GetExtent();
		VkExtent2D surfaceExtend = Context::pSurface->GetCurrentExtent();
		if (m_rebuildSwapchain || windowExtent.width != surfaceExtend.width || windowExtent.height != surfaceExtend.height)
		{
			m_rebuildSwapchain = false;
			Context::ResetFrameIndex();
			RebuildSwapchain();
		}

		// Wait for fence of previous frame with same frameIndex to finish:
		VKA(vkWaitForFences(Context::GetVkDevice(), 1, &m_fences[Context::frameIndex], VK_TRUE, UINT64_MAX));
		if (!AcquireImage())
			return 0;
		VKA(vkResetFences(Context::GetVkDevice(), 1, &m_fences[Context::frameIndex]));

		// Get draw calls:
		m_pDrawCalls = Graphics::GetSortedDrawCallPointers();

		// Record and submit current frame commands:
		m_commandPools[Context::frameIndex].ResetPool();
		RecordPreRenderComputeCommandBuffer();
		RecordShadowCommandBuffer();
		RecordForwardCommandBuffer();
		RecordPostRenderComputeCommandBuffer();
		RecordPresentCommandBuffer();
		SubmitCommandBuffers();

		// Reset engine data for next frame:
		compute::PreRender::ResetComputeCalls();
		Lighting::ResetLights();
		Graphics::ResetDrawCalls();

		if (!PresentImage())
			return 0;

		return 1;
	}



	// Private methods:
	void RenderCore::RebuildSwapchain()
	{
		// Wait for graphicsQueue to finish:
		VKA(vkQueueWaitIdle(Context::pLogicalDevice->GetGraphicsQueue().queue));

		// Recreate swapchain:
		std::unique_ptr<Swapchain> newSwapchain = std::make_unique<Swapchain>(Context::pLogicalDevice.get(), Context::pSurface.get(), VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, Context::pSwapchain.get());
		Context::pSwapchain.swap(newSwapchain);

		// Recreate renderpasses:
		RenderPassManager::RecreateRenderPasses();

		// Recreate synchronization objects:
		DestroyFences();
		DestroySemaphores();
		CreateFences();
		CreateSemaphores();
	}
	bool RenderCore::AcquireImage()
	{
		// Signal acquireSemaphore when done:
		VkResult result = vkAcquireNextImageKHR(Context::GetVkDevice(), Context::GetVkSwapchainKHR(), UINT64_MAX, m_acquireSemaphores[Context::frameIndex], VK_NULL_HANDLE, &m_imageIndex);

		// Resize if needed:
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || Context::pWindow->GetFramebufferResized())
		{
			Context::pWindow->SetFramebufferResized(false);
			m_rebuildSwapchain = true;
			return false;
		}
		else
		{
			VKA(result);
			return true;
		}
	}
	
	void RenderCore::RecordPreRenderComputeCommandBuffer()
	{
		// Begin command buffer:
		VkCommandBuffer commandBuffer = m_commandPools[Context::frameIndex].GetVkCommandBuffer(RenderStage::preRenderCompute);
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			ComputeShader* pComputeShader = nullptr;
			ComputeShader* pPreviousComputeShader = nullptr;
			VkPipeline pipeline = VK_NULL_HANDLE;
			VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
			ComputePushConstant pushConstant(Uint3::one, Time::GetTime(), Time::GetDeltaTime());

			for (compute::ComputeCall* computeCall : compute::PreRender::GetComputeCallPointers())
			{
				// Compute call is a barrier:
				if (computeCall->pComputeShader == nullptr)
				{
					VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
					memoryBarrier.srcStageMask = pipelineStage::computeShader;
					memoryBarrier.dstStageMask = pipelineStage::computeShader;
					memoryBarrier.srcAccessMask = computeCall->srcAccessMask;
					memoryBarrier.dstAccessMask = computeCall->dstAccessMask;

					VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
					dependencyInfo.memoryBarrierCount = 1;
					dependencyInfo.pMemoryBarriers = &memoryBarrier;

					vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
				}
				// Compute call is a dispatch:
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
						pushConstant.threadCount = computeCall->threadCount;
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

					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &computeCall->pShaderProperties->GetDescriptorSet(Context::frameIndex), 0, nullptr);
					vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
				}
			}

			// Release memory from compute shaders to vertex shaders:
			{
				VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
				memoryBarrier.srcStageMask = pipelineStage::computeShader;
				memoryBarrier.dstStageMask = pipelineStage::vertexShader;
				memoryBarrier.srcAccessMask = accessMask::computeShader::shaderWrite;
				memoryBarrier.dstAccessMask = accessMask::vertexShader::shaderRead;

				VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
				dependencyInfo.memoryBarrierCount = 1;
				dependencyInfo.pMemoryBarriers = &memoryBarrier;

				vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
			}
		}
		VKA(vkEndCommandBuffer(commandBuffer));
	}
	void RenderCore::RecordShadowCommandBuffer()
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


		// Begin command buffer:
		VkCommandBuffer commandBuffer = m_commandPools[Context::frameIndex].GetVkCommandBuffer(RenderStage::shadow);
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

							vkCmdBindVertexBuffers(commandBuffer, 0, 1, &pMesh->GetVertexBuffer()->GetVmaBuffer()->GetVkBuffer(), offsets);
							vkCmdBindIndexBuffer(commandBuffer, pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, Mesh::GetIndexType());

							vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowPipelineLayout, 0, 1, &drawCall->pShadowShaderProperties->GetDescriptorSet(Context::frameIndex), 0, nullptr);
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

							vkCmdBindVertexBuffers(commandBuffer, 0, 1, &pMesh->GetVertexBuffer()->GetVmaBuffer()->GetVkBuffer(), offsets);
							vkCmdBindIndexBuffer(commandBuffer, pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, Mesh::GetIndexType());

							vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowPipelineLayout, 0, 1, &drawCall->pShadowShaderProperties->GetDescriptorSet(Context::frameIndex), 0, nullptr);
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
	void RenderCore::RecordForwardCommandBuffer()
	{
		// Begin command buffer:
		VkCommandBuffer commandBuffer = m_commandPools[Context::frameIndex].GetVkCommandBuffer(RenderStage::forward);
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
					vkCmdBindIndexBuffer(commandBuffer, pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, Mesh::GetIndexType());

					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &drawCall->pShaderProperties->GetDescriptorSet(Context::frameIndex), 0, nullptr);
					vkCmdDrawIndexed(commandBuffer, 3 * pMesh->GetTriangleCount(), math::Max(drawCall->instanceCount, (uint32_t)1), 0, 0, 0);
				}
			}
			vkCmdEndRenderPass(commandBuffer);

			// Release memory from vertex shaders to compute shaders:
			{
				VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
				memoryBarrier.srcStageMask = pipelineStage::vertexShader;
				memoryBarrier.dstStageMask = pipelineStage::computeShader;
				memoryBarrier.srcAccessMask = accessMask::vertexShader::shaderRead;
				memoryBarrier.dstAccessMask = accessMask::computeShader::shaderWrite;

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
	void RenderCore::RecordPostRenderComputeCommandBuffer()
	{
		// Begin command buffer:
		VkCommandBuffer commandBuffer = m_commandPools[Context::frameIndex].GetVkCommandBuffer(RenderStage::postRenderCompute);
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
			for (compute::ComputeCall* computeCall : compute::PostRender::GetComputeCallPointers())
			{
				// Update shader specific data:
				if (callIndex % 2 == 0)
				{
					computeCall->pShaderProperties->SetTexture2d("inputImage", RenderPassManager::GetForwardRenderPass()->GetRenderTexture());
					computeCall->pShaderProperties->SetTexture2d("outputImage", RenderPassManager::GetForwardRenderPass()->GetSecondaryRenderTexture());
				}
				else
				{
					computeCall->pShaderProperties->SetTexture2d("inputImage", RenderPassManager::GetForwardRenderPass()->GetSecondaryRenderTexture());
					computeCall->pShaderProperties->SetTexture2d("outputImage", RenderPassManager::GetForwardRenderPass()->GetRenderTexture());
				}
				callIndex++;
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
					pushConstant.threadCount = computeCall->threadCount;
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

				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &computeCall->pShaderProperties->GetDescriptorSet(Context::frameIndex), 0, nullptr);
				vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);

				// All effects access the same inputImage and outputImage.
				// Thus add memory barrier between every effect:
				{
					VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
					memoryBarrier.srcStageMask = pipelineStage::computeShader;
					memoryBarrier.dstStageMask = pipelineStage::computeShader;
					memoryBarrier.srcAccessMask = accessMask::computeShader::shaderWrite;
					memoryBarrier.dstAccessMask = accessMask::computeShader::shaderRead;

					VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
					dependencyInfo.memoryBarrierCount = 1;
					dependencyInfo.pMemoryBarriers = &memoryBarrier;

					vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
				}
			}

			// Transition renderTexture image to shader read only optimal:
			{
				VkImageLayout newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				VkPipelineStageFlags2 srcStage = pipelineStage::computeShader;
				VkPipelineStageFlags2 dstStage = pipelineStage::fragmentShader;
				VkAccessFlags2 srcAccessMask = accessMask::computeShader::shaderWrite;
				VkAccessFlags2 dstAccessMask = accessMask::fragmentShader::shaderRead;
				RenderPassManager::GetForwardRenderPass()->GetRenderTexture()->GetVmaImage()->TransitionLayout(commandBuffer, newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
			}
		}
		VKA(vkEndCommandBuffer(commandBuffer));
	}
	void RenderCore::RecordPresentCommandBuffer()
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

		// Begin command buffer:
		VkCommandBuffer commandBuffer = m_commandPools[Context::frameIndex].GetVkCommandBuffer(RenderStage::present);
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			// Viewport and scissor:
			VkViewport viewport = {};
			viewport.width = (float)Context::pSurface->GetCurrentExtent().width;
			viewport.height = (float)Context::pSurface->GetCurrentExtent().height;
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
			renderPassBeginInfo.renderArea.extent = Context::pSurface->GetCurrentExtent();

			// Begin render pass:
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			{
				// Update shader specific data:
				pShaderProperties->SetTexture2d("renderTexture", RenderPassManager::GetForwardRenderPass()->GetRenderTexture());
				pShaderProperties->UpdateShaderData();

				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

				vkCmdBindVertexBuffers(commandBuffer, 0, bindingCount, pMaterial->GetMeshBuffers(pMesh), pMaterial->GetMeshOffsets(pMesh));
				vkCmdBindIndexBuffer(commandBuffer, pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, Mesh::GetIndexType());

				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &pShaderProperties->GetDescriptorSet(Context::frameIndex), 0, nullptr);
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

	void RenderCore::SubmitCommandBuffers()
	{
		// Ember::TODO: Try:
		// Compute shaders do not need to wait for aquire semaphore. Instead make them only wait for previous compute to finish.
		// Shadow shaders then wait for aquire and computeToShadow semaphore.

		// Pre render compute submission:
		{
			VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &m_acquireSemaphores[Context::frameIndex];
			submitInfo.pWaitDstStageMask = &waitStage;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_commandPools[Context::frameIndex].GetVkCommandBuffer(RenderStage::preRenderCompute);//  m_preRenderComputeCommands[Context::frameIndex].GetVkCommandBuffer();
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &m_preRenderComputeToShadowSemaphores[Context::frameIndex];
			VKA(vkQueueSubmit(Context::pLogicalDevice->GetGraphicsQueue().queue, 1, &submitInfo, nullptr));
		}

		// Shadow render pass submission:
		{
			VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &m_preRenderComputeToShadowSemaphores[Context::frameIndex];
			submitInfo.pWaitDstStageMask = &waitStage;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_commandPools[Context::frameIndex].GetVkCommandBuffer(RenderStage::shadow);//m_shadowCommands[Context::frameIndex].GetVkCommandBuffer();
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &m_shadowToForwardSemaphores[Context::frameIndex];
			VKA(vkQueueSubmit(Context::pLogicalDevice->GetGraphicsQueue().queue, 1, &submitInfo, nullptr));
		}

		// Forward render pass submission:
		{
			VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &m_shadowToForwardSemaphores[Context::frameIndex];
			submitInfo.pWaitDstStageMask = &waitStage;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_commandPools[Context::frameIndex].GetVkCommandBuffer(RenderStage::forward);//m_forwardCommands[Context::frameIndex].GetVkCommandBuffer();
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &m_forwardToPostRenderComputeSemaphores[Context::frameIndex];
			VKA(vkQueueSubmit(Context::pLogicalDevice->GetGraphicsQueue().queue, 1, &submitInfo, nullptr));
		}

		// Post render compute submission:
		{
			VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &m_forwardToPostRenderComputeSemaphores[Context::frameIndex];
			submitInfo.pWaitDstStageMask = &waitStage;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_commandPools[Context::frameIndex].GetVkCommandBuffer(RenderStage::postRenderCompute);//m_postRenderComputeCommands[Context::frameIndex].GetVkCommandBuffer();
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &m_postRenderToPresentSemaphores[Context::frameIndex];
			VKA(vkQueueSubmit(Context::pLogicalDevice->GetGraphicsQueue().queue, 1, &submitInfo, nullptr));
		}

		// Present render pass submission: (signal fence when done)
		{
			VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &m_postRenderToPresentSemaphores[Context::frameIndex];
			submitInfo.pWaitDstStageMask = &waitStage;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_commandPools[Context::frameIndex].GetVkCommandBuffer(RenderStage::present);//m_presentCommands[Context::frameIndex].GetVkCommandBuffer();
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &m_releaseSemaphores[Context::frameIndex];
			VKA(vkQueueSubmit(Context::pLogicalDevice->GetGraphicsQueue().queue, 1, &submitInfo, m_fences[Context::frameIndex]));
		}
	}
	bool RenderCore::PresentImage()
	{
		VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_releaseSemaphores[Context::frameIndex];	// wait for releaseSemaphor
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &Context::GetVkSwapchainKHR();
		presentInfo.pImageIndices = &m_imageIndex;

		VkResult result = vkQueuePresentKHR(Context::pLogicalDevice->GetPresentQueue().queue, &presentInfo);

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

	void RenderCore::CreateFences()
	{
		VkFenceCreateInfo createInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
		createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;	// Fence is created in the signaled state to prevent the first wait from blocking.

		m_fences.resize(Context::framesInFlight);
		for (uint32_t i = 0; i < Context::framesInFlight; i++)
			VKA(vkCreateFence(Context::GetVkDevice(), &createInfo, nullptr, &m_fences[i]));
	}
	void RenderCore::CreateSemaphores()
	{
		VkSemaphoreCreateInfo createInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

		m_acquireSemaphores.resize(Context::framesInFlight);
		m_preRenderComputeToShadowSemaphores.resize(Context::framesInFlight);
		m_shadowToForwardSemaphores.resize(Context::framesInFlight);
		m_forwardToPostRenderComputeSemaphores.resize(Context::framesInFlight);
		m_postRenderToPresentSemaphores.resize(Context::framesInFlight);
		m_releaseSemaphores.resize(Context::framesInFlight);
		for (uint32_t i = 0; i < Context::framesInFlight; i++)
		{
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_acquireSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_preRenderComputeToShadowSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_shadowToForwardSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_forwardToPostRenderComputeSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_postRenderToPresentSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_releaseSemaphores[i]));
		}
	}
	void RenderCore::DestroyFences()
	{
		for (uint32_t i = 0; i < Context::framesInFlight; i++)
			vkDestroyFence(Context::GetVkDevice(), m_fences[i], nullptr);
		m_fences.clear();
	}
	void RenderCore::DestroySemaphores()
	{
		for (uint32_t i = 0; i < Context::framesInFlight; i++)
		{
			vkDestroySemaphore(Context::GetVkDevice(), m_acquireSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_preRenderComputeToShadowSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_shadowToForwardSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_forwardToPostRenderComputeSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_postRenderToPresentSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_releaseSemaphores[i], nullptr);
		}
		m_acquireSemaphores.clear();
		m_preRenderComputeToShadowSemaphores.clear();
		m_shadowToForwardSemaphores.clear();
		m_forwardToPostRenderComputeSemaphores.clear();
		m_postRenderToPresentSemaphores.clear();
		m_releaseSemaphores.clear();
	}
}