#include "vulkanRenderer.h"
#include "compute.h"
#include "computePushConstant.h"
#include "computeShader.h"
#include "dearImGui.h"
#include "defaultPushConstant.h"
#include "directionalLight.h"
#include "drawCall.h"
#include "forwardRenderPass.h"
#include "graphics.h"
#include "macros.h"
#include "mesh.h"
#include "pipeline.h"
#include "pointLight.h"
#include "renderPassManager.h"
#include "renderTexture2d.h"
#include "scene.h"
#include "shaderProperties.h"
#include "shadowPushConstant.h"
#include "shadowRenderPass.h"
#include "spirvReflect.h"
#include "spotLight.h"
#include "transform.h"
#include "vmaBuffer.h"
#include "vulkanCommand.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"



namespace emberEngine
{
	// Constructor:
	VulkanRenderer::VulkanRenderer(VulkanContext* pContext)
	{
		m_pContext = pContext;
		m_pShadowMaterial = MaterialManager::GetMaterial("shadowMaterial");

		// Create render texture:
		VkFormat format = VK_FORMAT_R16G16B16A16_SFLOAT;
		int width = m_pContext->pWindow->GetWidth();
		int height = m_pContext->pWindow->GetHeight();
		m_renderTexture = std::make_unique<RenderTexture2d>(m_pContext, "mainRenderTexture2d", format, width, height);

		// Command buffers:
		m_syncComputeCommands.reserve(m_pContext->framesInFlight);
		m_shadowCommands.reserve(m_pContext->framesInFlight);
		m_forwardCommands.reserve(m_pContext->framesInFlight);
		for (uint32_t i = 0; i < m_pContext->framesInFlight; i++)
		{
			m_syncComputeCommands.emplace_back(m_pContext, m_pContext->pLogicalDevice->GetGraphicsQueue());
			m_shadowCommands.emplace_back(m_pContext, m_pContext->pLogicalDevice->GetGraphicsQueue());
			m_forwardCommands.emplace_back(m_pContext, m_pContext->pLogicalDevice->GetGraphicsQueue());
		}

		// Synchronization objects:
		CreateFences();
		CreateSemaphores();
	}



	// Destructor:
	VulkanRenderer::~VulkanRenderer()
	{
		VKA(vkDeviceWaitIdle(m_pContext->GetVkDevice()));
		DestroyFences();
		DestroySemaphores();
	}



	// Public methods:
	bool VulkanRenderer::RenderFrame(Scene* pScene)
	{
		// Resize Swapchain if needed:
		VkExtent2D windowExtent = m_pContext->pWindow->GetExtent();
		VkExtent2D surfaceExtend = m_pContext->pSurface->GetCurrentExtent();
		if (m_rebuildSwapchain || windowExtent.width != surfaceExtend.width || windowExtent.height != surfaceExtend.height)
		{
			m_rebuildSwapchain = false;
			m_pContext->ResetFrameIndex();
			RebuildSwapchain();
		}

		// Wait for fence of previous frame with same frameIndex to finish:
		VKA(vkWaitForFences(m_pContext->GetVkDevice(), 1, &m_fences[m_pContext->frameIndex], VK_TRUE, UINT64_MAX));
		if (!AcquireImage() || pScene->GetActiveCamera() == nullptr)
			return 0;
		VKA(vkResetFences(m_pContext->GetVkDevice(), 1, &m_fences[m_pContext->frameIndex]));

		m_pSyncComputeCalls = Compute::GetComputeCallPointers();
		m_pDrawCalls = Graphics::GetSortedDrawCallPointers();
		RecordComputeShaders(pScene);
		RecordShadowCommandBuffer(pScene);
		RecordForwardCommandBuffer(pScene);
		Compute::ResetComputeCalls();
		Graphics::ResetDrawCalls();
		SubmitCommandBuffers();
		if (!PresentImage())
			return 0;

		return 1;
	}
	const VulkanContext* const VulkanRenderer::GetContext() const
	{
		return m_pContext;
	}



	// Private:
	void VulkanRenderer::RebuildSwapchain()
	{
		// Wait for graphicsQueue to finish:
		VKA(vkQueueWaitIdle(m_pContext->pLogicalDevice->GetGraphicsQueue().queue));

		// Recreate swapchain:
		std::unique_ptr<VulkanSwapchain> newSwapchain = std::make_unique<VulkanSwapchain>(m_pContext->pLogicalDevice.get(), m_pContext->pSurface.get(), VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, m_pContext->pSwapchain.get());
		m_pContext->pSwapchain.swap(newSwapchain);

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
		VkResult result = vkAcquireNextImageKHR(m_pContext->GetVkDevice(), m_pContext->GetVkSwapchainKHR(), UINT64_MAX, m_acquireSemaphores[m_pContext->frameIndex], VK_NULL_HANDLE, &m_imageIndex);

		// Resize if needed:
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_pContext->pWindow->GetFramebufferResized())
		{
			m_pContext->pWindow->SetFramebufferResized(false);
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
		vkResetCommandPool(m_pContext->GetVkDevice(), m_syncComputeCommands[m_pContext->frameIndex].GetVkCommandPool(), 0);
		VkCommandBuffer commandBuffer = m_syncComputeCommands[m_pContext->frameIndex].GetVkCommandBuffer();

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

					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &computeCall->pShaderProperties->GetDescriptorSet(m_pContext->frameIndex), 0, nullptr);
					vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
				}
			}

			// Release memory barrier:
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
		VKA(vkEndCommandBuffer(commandBuffer));
	}
	void VulkanRenderer::RecordShadowCommandBuffer(Scene* pScene)
	{
		vkResetCommandPool(m_pContext->GetVkDevice(), m_shadowCommands[m_pContext->frameIndex].GetVkCommandPool(), 0);
		VkCommandBuffer commandBuffer = m_shadowCommands[m_pContext->frameIndex].GetVkCommandBuffer();
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
			renderPassBeginInfo.framebuffer = renderPass->GetFramebuffers()[m_pContext->frameIndex];
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent = VkExtent2D{ ShadowRenderPass::s_shadowMapWidth, ShadowRenderPass::s_shadowMapHeight };
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

				// Directional Lights:
				for (DirectionalLight* light : pScene->GetDirectionalLights())
				{
					if (light == nullptr)
						continue;
					light->UpdateShadowCascades(30.0f);

					for (uint32_t shadowCascadeIndex = 0; shadowCascadeIndex < (uint32_t)light->GetShadowCascadeCount(); shadowCascadeIndex++)
					{
						for (DrawCall* drawCall : *m_pDrawCalls)
						{
							if (drawCall->castShadows == false)
								continue;

							pMesh = drawCall->pMesh;

							// Update shader specific data (push constants):
							Float4x4 worldToClipMatrix = light->GetProjectionMatrix(shadowCascadeIndex) * light->GetViewMatrix(shadowCascadeIndex);
							ShadowPushConstant pushConstant(drawCall->instanceCount, shadowMapIndex, drawCall->localToWorldMatrix, worldToClipMatrix);
							vkCmdPushConstants(commandBuffer, shadowPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ShadowPushConstant), &pushConstant);

							vkCmdBindVertexBuffers(commandBuffer, 0, 1, &pMesh->GetVertexBuffer(m_pContext)->GetVkBuffer(), offsets);
							vkCmdBindIndexBuffer(commandBuffer, pMesh->GetIndexBuffer(m_pContext)->GetVkBuffer(), 0, Mesh::GetIndexType());

							vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowPipelineLayout, 0, 1, &drawCall->pShadowShaderProperties->GetDescriptorSet(m_pContext->frameIndex), 0, nullptr);
							vkCmdDrawIndexed(commandBuffer, 3 * pMesh->GetTriangleCount(), mathf::Max(drawCall->instanceCount, (uint32_t)1), 0, 0, 0);
						}
						shadowMapIndex++;
					}
				}

				// Spot Lights:
				for (SpotLight* light : pScene->GetSpotLights())
				{
					if (light == nullptr)
						continue;

					for (DrawCall* drawCall : *m_pDrawCalls)
					{
						if (drawCall->castShadows == false)
							continue;

						pMesh = drawCall->pMesh;

						// Update shader specific data (push constants):
						Float4x4 worldToClipMatrix = light->GetProjectionMatrix() * light->GetViewMatrix();
						ShadowPushConstant pushConstant(drawCall->instanceCount, shadowMapIndex, drawCall->localToWorldMatrix, worldToClipMatrix);
						vkCmdPushConstants(commandBuffer, shadowPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ShadowPushConstant), &pushConstant);

						vkCmdBindVertexBuffers(commandBuffer, 0, 1, &pMesh->GetVertexBuffer(m_pContext)->GetVkBuffer(), offsets);
						vkCmdBindIndexBuffer(commandBuffer, pMesh->GetIndexBuffer(m_pContext)->GetVkBuffer(), 0, Mesh::GetIndexType());

						vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowPipelineLayout, 0, 1, &drawCall->pShadowShaderProperties->GetDescriptorSet(m_pContext->frameIndex), 0, nullptr);
						vkCmdDrawIndexed(commandBuffer, 3 * pMesh->GetTriangleCount(), mathf::Max(drawCall->instanceCount, (uint32_t)1), 0, 0, 0);
					}
					shadowMapIndex++;
				}

				// Point Lights:
				for (PointLight* light : pScene->GetPointLights())
				{
					if (light == nullptr)
						continue;

					for (uint32_t faceIndex = 0; faceIndex < 6; faceIndex++)
					{
						for (DrawCall* drawCall : *m_pDrawCalls)
						{
							if (drawCall->castShadows == false)
								continue;

							pMesh = drawCall->pMesh;

							// Update shader specific data (push constants):
							Float4x4 worldToClipMatrix = light->GetProjectionMatrix() * light->GetViewMatrix(faceIndex);
							ShadowPushConstant pushConstant(drawCall->instanceCount, shadowMapIndex, drawCall->localToWorldMatrix, worldToClipMatrix);
							vkCmdPushConstants(commandBuffer, shadowPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ShadowPushConstant), &pushConstant);

							vkCmdBindVertexBuffers(commandBuffer, 0, 1, &pMesh->GetVertexBuffer(m_pContext)->GetVkBuffer(), offsets);
							vkCmdBindIndexBuffer(commandBuffer, pMesh->GetIndexBuffer(m_pContext)->GetVkBuffer(), 0, Mesh::GetIndexType());

							vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowPipelineLayout, 0, 1, &drawCall->pShadowShaderProperties->GetDescriptorSet(m_pContext->frameIndex), 0, nullptr);
							vkCmdDrawIndexed(commandBuffer, 3 * pMesh->GetTriangleCount(), mathf::Max(drawCall->instanceCount, (uint32_t)1), 0, 0, 0);
						}
						shadowMapIndex++;
					}
				}
			}
			vkCmdEndRenderPass(commandBuffer);
		}
		VKA(vkEndCommandBuffer(commandBuffer));
	}
	void VulkanRenderer::RecordForwardCommandBuffer(Scene* pScene)
	{
		vkResetCommandPool(m_pContext->GetVkDevice(), m_forwardCommands[m_pContext->frameIndex].GetVkCommandPool(), 0);
		VkCommandBuffer commandBuffer = m_forwardCommands[m_pContext->frameIndex].GetVkCommandBuffer();
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
			renderPassBeginInfo.renderArea.extent = m_pContext->pSurface->GetCurrentExtent();
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
				DefaultPushConstant pushConstant(0, Timer::GetTime(), Timer::GetDeltaTime(), pScene->GetDirectionalLightsCount(), pScene->GetSpotLightsCount(), pScene->GetPointLightsCount(), cameraPosition);

				// Normal draw calls:
				for (DrawCall* drawCall : *m_pDrawCalls)
				{
					pMesh = drawCall->pMesh;

					// Update shader specific data:
					drawCall->SetRenderMatrizes(pCamera);
					drawCall->SetLightData(pScene->GetDirectionalLights());
					drawCall->SetLightData(pScene->GetSpotLights());
					drawCall->SetLightData(pScene->GetPointLights());
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
					vkCmdBindIndexBuffer(commandBuffer, pMesh->GetIndexBuffer(m_pContext)->GetVkBuffer(), 0, Mesh::GetIndexType());

					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &drawCall->pShaderProperties->GetDescriptorSet(m_pContext->frameIndex), 0, nullptr);
					vkCmdDrawIndexed(commandBuffer, 3 * pMesh->GetTriangleCount(), mathf::Max(drawCall->instanceCount, (uint32_t)1), 0, 0, 0);
				}
			}
			DearImGui::Render(commandBuffer);
			vkCmdEndRenderPass(commandBuffer);

			// Release memory:
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
			submitInfo.pWaitSemaphores = &m_acquireSemaphores[m_pContext->frameIndex];				// wait for acquireSemaphor as these compute commands are synced with rendering
			submitInfo.pWaitDstStageMask = &waitStage;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_syncComputeCommands[m_pContext->frameIndex].GetVkCommandBuffer();
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &m_syncComputeToShadowSemaphores[m_pContext->frameIndex];// signal m_syncComputeToShadowSemaphores when done
			VKA(vkQueueSubmit(m_pContext->pLogicalDevice->GetGraphicsQueue().queue, 1, &submitInfo, nullptr)); // no fence needed (sync by semaphore)
		}

		// Shadow render pass submission:
		{
			VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;			// wait at depth and stencil test stage until semaphores are signaled
			VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &m_syncComputeToShadowSemaphores[m_pContext->frameIndex];	// wait for synchronized compute commands to finish
			submitInfo.pWaitDstStageMask = &waitStage;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_shadowCommands[m_pContext->frameIndex].GetVkCommandBuffer();
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &m_shadowToForwardSemaphores[m_pContext->frameIndex];	// signal shadowToForwardSemaphore when done
			VKA(vkQueueSubmit(m_pContext->pLogicalDevice->GetGraphicsQueue().queue, 1, &submitInfo, nullptr)); // no fence needed (sync by semaphore)
		}

		// Forward render pass submission:
		{
			VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;					// wait at fragment shader stage until semaphores are signaled
			VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &m_shadowToForwardSemaphores[m_pContext->frameIndex];		// wait for shadowToForwardSemaphore
			submitInfo.pWaitDstStageMask = &waitStage;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_forwardCommands[m_pContext->frameIndex].GetVkCommandBuffer();
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &m_releaseSemaphores[m_pContext->frameIndex];			// signal releaseSemaphor when done
			VKA(vkQueueSubmit(m_pContext->pLogicalDevice->GetGraphicsQueue().queue, 1, &submitInfo, m_fences[m_pContext->frameIndex])); // signal fence when done
		}
	}

	bool VulkanRenderer::PresentImage()
	{
		VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_releaseSemaphores[m_pContext->frameIndex]; // wait for releaseSemaphor
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_pContext->GetVkSwapchainKHR();
		presentInfo.pImageIndices = &m_imageIndex;

		VkResult result = vkQueuePresentKHR(m_pContext->pLogicalDevice->GetPresentQueue().queue, &presentInfo);

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
		viewport.width = (float)m_pContext->pSurface->GetCurrentExtent().width;
		viewport.height = (float)m_pContext->pSurface->GetCurrentExtent().height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.extent = m_pContext->pSurface->GetCurrentExtent();

		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void VulkanRenderer::CreateFences()
	{
		VkFenceCreateInfo createInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
		createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;	// Fence is created in the signaled state to prevent the first wait from blocking.

		m_fences.resize(m_pContext->framesInFlight);
		for (uint32_t i = 0; i < m_pContext->framesInFlight; i++)
			VKA(vkCreateFence(m_pContext->GetVkDevice(), &createInfo, nullptr, &m_fences[i]));
	}

	void VulkanRenderer::CreateSemaphores()
	{
		VkSemaphoreCreateInfo createInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

		m_acquireSemaphores.resize(m_pContext->framesInFlight);
		m_syncComputeToShadowSemaphores.resize(m_pContext->framesInFlight);
		m_shadowToForwardSemaphores.resize(m_pContext->framesInFlight);
		m_releaseSemaphores.resize(m_pContext->framesInFlight);
		for (uint32_t i = 0; i < m_pContext->framesInFlight; i++)
		{
			VKA(vkCreateSemaphore(m_pContext->GetVkDevice(), &createInfo, nullptr, &m_acquireSemaphores[i]));
			VKA(vkCreateSemaphore(m_pContext->GetVkDevice(), &createInfo, nullptr, &m_syncComputeToShadowSemaphores[i]));
			VKA(vkCreateSemaphore(m_pContext->GetVkDevice(), &createInfo, nullptr, &m_shadowToForwardSemaphores[i]));
			VKA(vkCreateSemaphore(m_pContext->GetVkDevice(), &createInfo, nullptr, &m_releaseSemaphores[i]));
		}
	}

	void VulkanRenderer::DestroyFences()
	{
		for (uint32_t i = 0; i < m_pContext->framesInFlight; i++)
			vkDestroyFence(m_pContext->GetVkDevice(), m_fences[i], nullptr);
		m_fences.clear();
	}

	void VulkanRenderer::DestroySemaphores()
	{
		for (uint32_t i = 0; i < m_pContext->framesInFlight; i++)
		{
			vkDestroySemaphore(m_pContext->GetVkDevice(), m_acquireSemaphores[i], nullptr);
			vkDestroySemaphore(m_pContext->GetVkDevice(), m_syncComputeToShadowSemaphores[i], nullptr);
			vkDestroySemaphore(m_pContext->GetVkDevice(), m_shadowToForwardSemaphores[i], nullptr);
			vkDestroySemaphore(m_pContext->GetVkDevice(), m_releaseSemaphores[i], nullptr);
		}
		m_acquireSemaphores.clear();
		m_syncComputeToShadowSemaphores.clear();
		m_shadowToForwardSemaphores.clear();
		m_releaseSemaphores.clear();
	}
}