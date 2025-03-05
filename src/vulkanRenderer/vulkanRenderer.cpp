#include "vulkanRenderer.h"
#include "camera.h"
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
#include "pipeline.h"
#include "renderPassManager.h"
#include "renderTexture2d.h"
#include "scene.h"
#include "shaderProperties.h"
#include "shadowPushConstant.h"
#include "shadowRenderPass.h"
#include "spirvReflect.h"
#include "timer.h"
#include "vmaBuffer.h"
#include "vulkanCommand.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"



namespace emberEngine
{
	// Constructor:
	VulkanRenderer::VulkanRenderer()
	{
		m_pShadowMaterial = MaterialManager::GetMaterial("shadowMaterial");

		// Create render texture:
		VkFormat format = VK_FORMAT_R16G16B16A16_SFLOAT;
		int width = VulkanContext::pWindow->GetWidth();
		int height = VulkanContext::pWindow->GetHeight();
		m_renderTexture = std::make_unique<RenderTexture2d>("mainRenderTexture2d", format, width, height);

		// Command buffers:
		m_syncComputeCommands.reserve(VulkanContext::framesInFlight);
		m_shadowCommands.reserve(VulkanContext::framesInFlight);
		m_forwardCommands.reserve(VulkanContext::framesInFlight);
		for (uint32_t i = 0; i < VulkanContext::framesInFlight; i++)
		{
			m_syncComputeCommands.emplace_back(VulkanContext::pLogicalDevice->GetGraphicsQueue());
			m_shadowCommands.emplace_back(VulkanContext::pLogicalDevice->GetGraphicsQueue());
			m_forwardCommands.emplace_back(VulkanContext::pLogicalDevice->GetGraphicsQueue());
		}

		// Synchronization objects:
		CreateFences();
		CreateSemaphores();

		// Injection systems initilization:
		Compute::Init();
		Lighting::Init();
		Graphics::Init();
	}



	// Destructor:
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
	bool VulkanRenderer::RenderFrame(Scene* pScene)
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
		if (!AcquireImage() || pScene->GetActiveCamera() == nullptr)
			return 0;
		VKA(vkResetFences(VulkanContext::GetVkDevice(), 1, &m_fences[VulkanContext::frameIndex]));

		// Get compute and draw calls:
		m_pSyncComputeCalls = Compute::GetComputeCallPointers();
		m_pDrawCalls = Graphics::GetSortedDrawCallPointers();

		// Record and submit frame:
		RecordComputeShaders(pScene);
		RecordShadowCommandBuffer(pScene);
		RecordForwardCommandBuffer(pScene);
		SubmitCommandBuffers();

		// Reset engine data for next frame:
		Compute::ResetComputeCalls();
		Lighting::ResetLights();
		Graphics::ResetDrawCalls();

		if (!PresentImage())
			return 0;

		return 1;
	}



	// Private:
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
	
	void VulkanRenderer::RecordComputeShaders(Scene* pScene)
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
			ComputePushConstant pushConstant(Uint3::one, Timer::GetTime(), Timer::GetDeltaTime());

			for (ComputeCall* computeCall : *m_pSyncComputeCalls)
			{
				// Compute call is a barrier:
				if (computeCall->pComputeShader == nullptr)
				{
					VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
					memoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
					memoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
					memoryBarrier.srcAccessMask = computeCall->srcAccessMask;
					memoryBarrier.dstAccessMask = computeCall->dstAccessMask;

					VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
					dependencyInfo.memoryBarrierCount = 1;
					dependencyInfo.pMemoryBarriers = &memoryBarrier;

					vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
				}
				// Compute call is dispatch call:
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
						ComputePushConstant pushConstant(computeCall->threadCount, Timer::GetTime(), Timer::GetDeltaTime());
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
				memoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
				memoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
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
	void VulkanRenderer::RecordShadowCommandBuffer(Scene* pScene)
	{
		vkResetCommandPool(VulkanContext::GetVkDevice(), m_shadowCommands[VulkanContext::frameIndex].GetVkCommandPool(), 0);
		VkCommandBuffer commandBuffer = m_shadowCommands[VulkanContext::frameIndex].GetVkCommandBuffer();
		Camera* pCamera = pScene->GetActiveCamera();
		if (pCamera == nullptr)
			return;

		// Begin command buffer:
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			// Render pass info:
			ShadowRenderPass* renderPass = dynamic_cast<ShadowRenderPass*>(RenderPassManager::GetRenderPass("shadowRenderPass"));
			VkClearValue clearValues = {};
			clearValues.depthStencil = { 1.0f, 0 };
			VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
			renderPassBeginInfo.renderPass = renderPass->GetVkRenderPass();
			renderPassBeginInfo.framebuffer = renderPass->GetFramebuffers()[VulkanContext::frameIndex];
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent = VkExtent2D{ Lighting::shadowMapResolution, Lighting::shadowMapResolution };
			renderPassBeginInfo.clearValueCount = 1;
			renderPassBeginInfo.pClearValues = &clearValues;
			const VkDeviceSize offsets[1] = { 0 };

			// Begin render pass:
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			{
				Mesh* pMesh = nullptr;
				const VkPipeline& shadowPipeline = m_pShadowMaterial->GetPipeline()->GetVkPipeline();
				const VkPipelineLayout& shadowPipelineLayout = m_pShadowMaterial->GetPipeline()->GetVkPipelineLayout();
				int shadowMapIndex = 0;
				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowPipeline);

				// Update shader specific data:
				for (DrawCall* drawCall : *m_pDrawCalls)
					drawCall->pShadowShaderProperties->UpdateShaderData();

				//// Directional Lights:
				//std::array<Lighting::DirectionalLight, Lighting::maxDirectionalLights>& directionalLights = Lighting::GetDirectionalLights();
				//for (int i = 0; i < Lighting::GetDirectionalLightsCount(); i++)
				//{
				//	light->UpdateShadowCascades(30.0f);
				//
				//	for (uint32_t shadowCascadeIndex = 0; shadowCascadeIndex < (uint32_t)light->GetShadowCascadeCount(); shadowCascadeIndex++)
				//	{
				//		for (DrawCall* drawCall : *m_pDrawCalls)
				//		{
				//			if (drawCall->castShadows == false)
				//				continue;
				//
				//			pMesh = drawCall->pMesh;
				//
				//			// Update shader specific data (push constants):
				//			Float4x4 worldToClipMatrix = light->GetProjectionMatrix(shadowCascadeIndex) * light->GetViewMatrix(shadowCascadeIndex);
				//			ShadowPushConstant pushConstant(drawCall->instanceCount, shadowMapIndex, drawCall->localToWorldMatrix, worldToClipMatrix);
				//			vkCmdPushConstants(commandBuffer, shadowPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ShadowPushConstant), &pushConstant);
				//
				//			vkCmdBindVertexBuffers(commandBuffer, 0, 1, &pMesh->GetVertexBuffer()->GetVkBuffer(), offsets);
				//			vkCmdBindIndexBuffer(commandBuffer, pMesh->GetIndexBuffer()->GetVkBuffer(), 0, Mesh::GetIndexType());
				//
				//			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowPipelineLayout, 0, 1, &drawCall->pShadowShaderProperties->GetDescriptorSet(VulkanContext::frameIndex), 0, nullptr);
				//			vkCmdDrawIndexed(commandBuffer, 3 * pMesh->GetTriangleCount(), math::Max(drawCall->instanceCount, (uint32_t)1), 0, 0, 0);
				//		}
				//		shadowMapIndex++;
				//	}
				//}

				// Positional Lights:
				std::array<Lighting::PositionalLight, Lighting::maxPositionalLights>& positionalLights = Lighting::GetPositionalLights();
				for (int i = 0; i < Lighting::GetPositionalLightsCount(); i++)
				{
					Lighting::PositionalLight& light = positionalLights[i];
					for (DrawCall* drawCall : *m_pDrawCalls)
					{
						if (drawCall->castShadows == false)
							continue;

						pMesh = drawCall->pMesh;

						// Update shader specific data (push constants):
						Float4x4 worldToClipMatrix = light.projectionMatrix * light.viewMatrix;
						ShadowPushConstant pushConstant(drawCall->instanceCount, shadowMapIndex, drawCall->localToWorldMatrix, worldToClipMatrix);
						vkCmdPushConstants(commandBuffer, shadowPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ShadowPushConstant), &pushConstant);

						vkCmdBindVertexBuffers(commandBuffer, 0, 1, &pMesh->GetVertexBuffer()->GetVkBuffer(), offsets);
						vkCmdBindIndexBuffer(commandBuffer, pMesh->GetIndexBuffer()->GetVkBuffer(), 0, Mesh::GetIndexType());

						vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowPipelineLayout, 0, 1, &drawCall->pShadowShaderProperties->GetDescriptorSet(VulkanContext::frameIndex), 0, nullptr);
						vkCmdDrawIndexed(commandBuffer, 3 * pMesh->GetTriangleCount(), math::Max(drawCall->instanceCount, (uint32_t)1), 0, 0, 0);
					}
					shadowMapIndex++;
				}
			}
			vkCmdEndRenderPass(commandBuffer);
		}
		VKA(vkEndCommandBuffer(commandBuffer));
	}
	void VulkanRenderer::RecordForwardCommandBuffer(Scene* pScene)
	{
		vkResetCommandPool(VulkanContext::GetVkDevice(), m_forwardCommands[VulkanContext::frameIndex].GetVkCommandPool(), 0);
		VkCommandBuffer commandBuffer = m_forwardCommands[VulkanContext::frameIndex].GetVkCommandBuffer();
		Camera* pCamera = pScene->GetActiveCamera();
		if (pCamera == nullptr)
			return;

		// Begin command buffer:
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			// Viewport and Scissor:
			SetViewportAndScissor(commandBuffer);

			// Render pass info:
			ForwardRenderPass* renderPass = dynamic_cast<ForwardRenderPass*>(RenderPassManager::GetRenderPass("forwardRenderPass"));
			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
			clearValues[1].depthStencil = { 1.0f, 0 };
			VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
			renderPassBeginInfo.renderPass = renderPass->GetVkRenderPass();
			renderPassBeginInfo.framebuffer = renderPass->GetFramebuffers()[m_imageIndex];
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent = VulkanContext::pSurface->GetCurrentExtent();
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
				Float3 cameraPosition = pCamera->GetTransform()->GetPosition();
				DefaultPushConstant pushConstant(0, Timer::GetTime(), Timer::GetDeltaTime(), Lighting::GetDirectionalLightsCount(), Lighting::GetPositionalLightsCount(), cameraPosition);

				// Normal draw calls:
				for (DrawCall* drawCall : *m_pDrawCalls)
				{
					pMesh = drawCall->pMesh;

					// Update shader specific data:
					drawCall->SetRenderMatrizes(pCamera);
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
			DearImGui::Render(commandBuffer);
			vkCmdEndRenderPass(commandBuffer);

			// Release memory from vertex shaders to compute shaders:
			{
				VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
				memoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
				memoryBarrier.srcAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
				memoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
				memoryBarrier.dstAccessMask = VK_ACCESS_2_SHADER_WRITE_BIT;

				VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
				dependencyInfo.memoryBarrierCount = 1;
				dependencyInfo.pMemoryBarriers = &memoryBarrier;

				vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
			}
		}
		VKA(vkEndCommandBuffer(commandBuffer));
	}

	void VulkanRenderer::SubmitCommandBuffers()
	{
		// Synchronized pre graphics compute submission:
		{
			VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;					// wait at compute stage until semaphores are signaled
			VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &m_acquireSemaphores[VulkanContext::frameIndex];				// wait for acquireSemaphor as these compute commands are synced with rendering
			submitInfo.pWaitDstStageMask = &waitStage;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_syncComputeCommands[VulkanContext::frameIndex].GetVkCommandBuffer();
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &m_syncComputeToShadowSemaphores[VulkanContext::frameIndex];// signal m_syncComputeToShadowSemaphores when done
			VKA(vkQueueSubmit(VulkanContext::pLogicalDevice->GetGraphicsQueue().queue, 1, &submitInfo, nullptr)); // no fence needed (sync by semaphore)
		}

		// Shadow render pass submission:
		{
			VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;			// wait at depth and stencil test stage until semaphores are signaled
			VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &m_syncComputeToShadowSemaphores[VulkanContext::frameIndex];	// wait for synchronized compute commands to finish
			submitInfo.pWaitDstStageMask = &waitStage;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_shadowCommands[VulkanContext::frameIndex].GetVkCommandBuffer();
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &m_shadowToForwardSemaphores[VulkanContext::frameIndex];	// signal shadowToForwardSemaphore when done
			VKA(vkQueueSubmit(VulkanContext::pLogicalDevice->GetGraphicsQueue().queue, 1, &submitInfo, nullptr)); // no fence needed (sync by semaphore)
		}

		// Forward render pass submission:
		{
			VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;					// wait at fragment shader stage until semaphores are signaled
			VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &m_shadowToForwardSemaphores[VulkanContext::frameIndex];		// wait for shadowToForwardSemaphore
			submitInfo.pWaitDstStageMask = &waitStage;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_forwardCommands[VulkanContext::frameIndex].GetVkCommandBuffer();
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &m_releaseSemaphores[VulkanContext::frameIndex];			// signal releaseSemaphor when done
			VKA(vkQueueSubmit(VulkanContext::pLogicalDevice->GetGraphicsQueue().queue, 1, &submitInfo, m_fences[VulkanContext::frameIndex])); // signal fence when done
		}
	}

	bool VulkanRenderer::PresentImage()
	{
		VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_releaseSemaphores[VulkanContext::frameIndex]; // wait for releaseSemaphor
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

	void VulkanRenderer::SetViewportAndScissor(VkCommandBuffer& commandBuffer)
	{
		VkViewport viewport = {};
		viewport.width = (float)VulkanContext::pSurface->GetCurrentExtent().width;
		viewport.height = (float)VulkanContext::pSurface->GetCurrentExtent().height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.extent = VulkanContext::pSurface->GetCurrentExtent();

		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
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
		m_releaseSemaphores.resize(VulkanContext::framesInFlight);
		for (uint32_t i = 0; i < VulkanContext::framesInFlight; i++)
		{
			VKA(vkCreateSemaphore(VulkanContext::GetVkDevice(), &createInfo, nullptr, &m_acquireSemaphores[i]));
			VKA(vkCreateSemaphore(VulkanContext::GetVkDevice(), &createInfo, nullptr, &m_syncComputeToShadowSemaphores[i]));
			VKA(vkCreateSemaphore(VulkanContext::GetVkDevice(), &createInfo, nullptr, &m_shadowToForwardSemaphores[i]));
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
			vkDestroySemaphore(VulkanContext::GetVkDevice(), m_releaseSemaphores[i], nullptr);
		}
		m_acquireSemaphores.clear();
		m_syncComputeToShadowSemaphores.clear();
		m_shadowToForwardSemaphores.clear();
		m_releaseSemaphores.clear();
	}
}