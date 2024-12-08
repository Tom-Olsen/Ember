#include "vulkanRenderer.h"
#include "graphics.h"
#include "macros.h"
#include "renderPassManager.h"
#include "scene.h"
#include "shadingPushConstant.h"
#include "shadowPushConstant.h"
#include "vulkanCommand.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"



// Constructor:
VulkanRenderer::VulkanRenderer(VulkanContext* pContext)
{
	m_pContext = pContext;

	// Command buffers:
	m_shadowCommands.reserve(m_pContext->framesInFlight);
	m_shadingCommands.reserve(m_pContext->framesInFlight);
	for (uint32_t i = 0; i < m_pContext->framesInFlight; i++)
	{
		m_shadowCommands.emplace_back(m_pContext, m_pContext->pLogicalDevice->GetGraphicsQueue());
		m_shadingCommands.emplace_back(m_pContext, m_pContext->pLogicalDevice->GetGraphicsQueue());
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
bool VulkanRenderer::Render(Scene* pScene)
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
	if (!AcquireImage() || pScene->activeCamera == nullptr)
		return 0;
	VKA(vkResetFences(m_pContext->GetVkDevice(), 1, &m_fences[m_pContext->frameIndex]));

	SetMeshRendererGroups(pScene);
	RecordShadowCommandBuffer(pScene);
	RecordShadingCommandBuffer(pScene);
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

void VulkanRenderer::SetMeshRendererGroups(Scene* pScene)
{
	m_pMeshRendererGroups[0] = pScene->GetSortedMeshRenderers();
	m_pMeshRendererGroups[1] = Graphics::GetSortedMeshRenderers();
}

void VulkanRenderer::RecordShadowCommandBuffer(Scene* pScene)
{
	vkResetCommandPool(m_pContext->GetVkDevice(), m_shadowCommands[m_pContext->frameIndex].GetVkCommandPool(), 0);
	VkCommandBuffer commandBuffer = m_shadowCommands[m_pContext->frameIndex].GetVkCommandBuffer();

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
		renderPassBeginInfo.renderPass = renderPass->renderPass;
		renderPassBeginInfo.framebuffer = renderPass->framebuffers[m_pContext->frameIndex];
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = VkExtent2D{ ShadowRenderPass::shadowMapWidth, ShadowRenderPass::shadowMapHeight };
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearValues;

		// Begin render pass:
		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		{
			int shadowMapIndex = 0;
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, MeshRenderer::GetShadowPipeline());
			
			// Directional Lights:
			for (auto& light : pScene->directionalLights)
			{
				if (light == nullptr)
					continue;

				for (auto group : m_pMeshRendererGroups)
					for (MeshRenderer* meshRenderer : *group)
					{
						if (meshRenderer->IsActive() && meshRenderer->castShadows)
						{
							// Update shader specific data (push constants and uniform buffers):
							Float4x4 localToClipMatrix = light->GetProjectionMatrix() * light->GetViewMatrix() * meshRenderer->transform->GetLocalToWorldMatrix();
							ShadowPushConstant pushConstant(shadowMapIndex, localToClipMatrix);
							vkCmdPushConstants(commandBuffer, MeshRenderer::GetShadowPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ShadowPushConstant), &pushConstant);

							const VkDeviceSize offsets[1] = { 0 };
							vkCmdBindVertexBuffers(commandBuffer, 0, 1, &meshRenderer->mesh->GetVertexBuffer(m_pContext)->GetVkBuffer(), offsets);
							vkCmdBindIndexBuffer(commandBuffer, meshRenderer->mesh->GetIndexBuffer(m_pContext)->GetVkBuffer(), 0, Mesh::GetIndexType());

							vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, meshRenderer->GetShadowPipelineLayout(), 0, 1, meshRenderer->GetShadowDescriptorSets(m_pContext->frameIndex), 0, nullptr);
							vkCmdDrawIndexed(commandBuffer, 3 * meshRenderer->mesh->GetTriangleCount(), 1, 0, 0, 0);
						}
					}
				shadowMapIndex++;
			}

			// Spot Lights:
			for (auto& light : pScene->spotLights)
			{
				if (light == nullptr)
					continue;

				for (auto group : m_pMeshRendererGroups)
					for (MeshRenderer* meshRenderer : *group)
					{
						if (meshRenderer->IsActive() && meshRenderer->castShadows)
						{
							// Update shader specific data (push constants and uniform buffers):
							Float4x4 localToClipMatrix = light->GetProjectionMatrix() * light->GetViewMatrix() * meshRenderer->transform->GetLocalToWorldMatrix();
							ShadowPushConstant pushConstant(shadowMapIndex, localToClipMatrix);
							vkCmdPushConstants(commandBuffer, MeshRenderer::GetShadowPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ShadowPushConstant), &pushConstant);

							const VkDeviceSize offsets[1] = { 0 };
							vkCmdBindVertexBuffers(commandBuffer, 0, 1, &meshRenderer->mesh->GetVertexBuffer(m_pContext)->GetVkBuffer(), offsets);
							vkCmdBindIndexBuffer(commandBuffer, meshRenderer->mesh->GetIndexBuffer(m_pContext)->GetVkBuffer(), 0, Mesh::GetIndexType());

							vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, meshRenderer->GetShadowPipelineLayout(), 0, 1, meshRenderer->GetShadowDescriptorSets(m_pContext->frameIndex), 0, nullptr);
							vkCmdDrawIndexed(commandBuffer, 3 * meshRenderer->mesh->GetTriangleCount(), 1, 0, 0, 0);
						}
					}
				shadowMapIndex++;
			}

			// Point Lights:
			for (auto& light : pScene->pointLights)
			{
				if (light == nullptr)
					continue;

				for (uint32_t faceIndex = 0; faceIndex < 6; faceIndex++)
				{
					for (auto group : m_pMeshRendererGroups)
						for (MeshRenderer* meshRenderer : *group)
						{
							if (meshRenderer->IsActive() && meshRenderer->castShadows)
							{
								// Update shader specific data (push constants and uniform buffers):
								Float4x4 localToClipMatrix = light->GetProjectionMatrix() * light->GetViewMatrix(faceIndex) * meshRenderer->transform->GetLocalToWorldMatrix();
								ShadowPushConstant pushConstant(shadowMapIndex, localToClipMatrix);
								vkCmdPushConstants(commandBuffer, MeshRenderer::GetShadowPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ShadowPushConstant), &pushConstant);

								const VkDeviceSize offsets[1] = { 0 };
								vkCmdBindVertexBuffers(commandBuffer, 0, 1, &meshRenderer->mesh->GetVertexBuffer(m_pContext)->GetVkBuffer(), offsets);
								vkCmdBindIndexBuffer(commandBuffer, meshRenderer->mesh->GetIndexBuffer(m_pContext)->GetVkBuffer(), 0, Mesh::GetIndexType());

								vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, meshRenderer->GetShadowPipelineLayout(), 0, 1, meshRenderer->GetShadowDescriptorSets(m_pContext->frameIndex), 0, nullptr);
								vkCmdDrawIndexed(commandBuffer, 3 * meshRenderer->mesh->GetTriangleCount(), 1, 0, 0, 0);
							}
						}
					shadowMapIndex++;
				}
			}
		}
		vkCmdEndRenderPass(commandBuffer);
	}
	VKA(vkEndCommandBuffer(commandBuffer));
}
void VulkanRenderer::RecordShadingCommandBuffer(Scene* pScene)
{
	vkResetCommandPool(m_pContext->GetVkDevice(), m_shadingCommands[m_pContext->frameIndex].GetVkCommandPool(), 0);
	VkCommandBuffer commandBuffer = m_shadingCommands[m_pContext->frameIndex].GetVkCommandBuffer();

	// Begin command buffer:
	VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
	{
		// Viewport and Scissor:
		SetViewportAndScissor(commandBuffer);

		// Render pass info:
		ShadingRenderPass* renderPass = dynamic_cast<ShadingRenderPass*>(RenderPassManager::GetRenderPass("shadingRenderPass"));
		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		renderPassBeginInfo.renderPass = renderPass->renderPass;
		renderPassBeginInfo.framebuffer = renderPass->framebuffers[m_imageIndex];
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = m_pContext->pSurface->GetCurrentExtent();
		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassBeginInfo.pClearValues = clearValues.data();

		// Begin render pass:
		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		{
			Float3 cameraPosition = pScene->activeCamera->transform->GetPosition();
			ShadingPushConstant pushConstant(Timer::GetTime(), Timer::GetDeltaTime(), pScene->dLightsCount, pScene->sLightsCount, pScene->pLightsCount, cameraPosition);

			Material* previousMaterial = nullptr;
			for (auto group : m_pMeshRendererGroups)
				for (MeshRenderer* meshRenderer : *group)
				{
					if (meshRenderer->IsActive())
					{
						meshRenderer->SetRenderMatrizes(pScene->activeCamera);
						meshRenderer->SetLightData(pScene->directionalLights);
						meshRenderer->SetLightData(pScene->spotLights);
						meshRenderer->SetLightData(pScene->pointLights);
						meshRenderer->materialProperties->UpdateShaderData();

						if (previousMaterial != meshRenderer->GetMaterial())
						{
							previousMaterial = meshRenderer->GetMaterial();
							vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, meshRenderer->GetShadingPipeline());
							vkCmdPushConstants(commandBuffer, meshRenderer->GetShadingPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(ShadingPushConstant), &pushConstant);
						}

						vkCmdBindVertexBuffers(commandBuffer, 0, meshRenderer->mesh->GetBindingCount(), meshRenderer->mesh->GetBuffers(m_pContext), meshRenderer->mesh->GetOffsets());
						vkCmdBindIndexBuffer(commandBuffer, meshRenderer->mesh->GetIndexBuffer(m_pContext)->GetVkBuffer(), 0, Mesh::GetIndexType());

						vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, meshRenderer->GetShadingPipelineLayout(), 0, 1, meshRenderer->GetShadingDescriptorSets(m_pContext->frameIndex), 0, nullptr);
						vkCmdDrawIndexed(commandBuffer, 3 * meshRenderer->mesh->GetTriangleCount(), 1, 0, 0, 0);
					}
				}
		}
		vkCmdEndRenderPass(commandBuffer);
	}
	VKA(vkEndCommandBuffer(commandBuffer));
}

void VulkanRenderer::SubmitCommandBuffers()
{
	// Shadow render pass:
	{
		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;	// wait at depth and stencil test stage
		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &m_acquireSemaphores[m_pContext->frameIndex];			// wait for acquireSemaphor
		submitInfo.pWaitDstStageMask = &waitStage;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_shadowCommands[m_pContext->frameIndex].GetVkCommandBuffer();
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_shadowToShadingSemaphores[m_pContext->frameIndex]; // signal shadowToShadingSemaphore when done
		VKA(vkQueueSubmit(m_pContext->pLogicalDevice->GetGraphicsQueue().queue, 1, &submitInfo, nullptr)); // signal fence when done
	}

	// Shading render pass:
	{
		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;			// wait at fragment shader stage
		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &m_shadowToShadingSemaphores[m_pContext->frameIndex];	// wait for shadowToShadingSemaphore
		submitInfo.pWaitDstStageMask = &waitStage;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_shadingCommands[m_pContext->frameIndex].GetVkCommandBuffer();
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
	m_shadowToShadingSemaphores.resize(m_pContext->framesInFlight);
	m_releaseSemaphores.resize(m_pContext->framesInFlight);
	for (uint32_t i = 0; i < m_pContext->framesInFlight; i++)
	{
		VKA(vkCreateSemaphore(m_pContext->GetVkDevice(), &createInfo, nullptr, &m_acquireSemaphores[i]));
		VKA(vkCreateSemaphore(m_pContext->GetVkDevice(), &createInfo, nullptr, &m_shadowToShadingSemaphores[i]));
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
		vkDestroySemaphore(m_pContext->GetVkDevice(), m_shadowToShadingSemaphores[i], nullptr);
		vkDestroySemaphore(m_pContext->GetVkDevice(), m_releaseSemaphores[i], nullptr);
	}
	m_acquireSemaphores.clear();
	m_shadowToShadingSemaphores.clear();
	m_releaseSemaphores.clear();
}