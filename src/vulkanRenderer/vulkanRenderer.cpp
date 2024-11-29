#include "vulkanRenderer.h"
#include "vulkanMacros.h"
#include "shadingPushConstant.h"
#include "shadowPushConstant.h"
#include "renderPassManager.h"
#include "macros.h"



// Constructor:
VulkanRenderer::VulkanRenderer(VulkanContext* context)
{
	this->context = context;

	// Command buffers:
	shadowCommands.reserve(context->framesInFlight);
	shadingCommands.reserve(context->framesInFlight);
	for (uint32_t i = 0; i < context->framesInFlight; i++)
	{
		shadowCommands.emplace_back(context, context->logicalDevice->graphicsQueue);
		shadingCommands.emplace_back(context, context->logicalDevice->graphicsQueue);
	}

	// Synchronization objects:
	CreateFences();
	CreateSemaphores();
}



// Destructor:
VulkanRenderer::~VulkanRenderer()
{
	VKA(vkDeviceWaitIdle(context->LogicalDevice()));
	DestroyFences();
	DestroySemaphores();
}



// Public methods:
bool VulkanRenderer::Render(Scene* scene)
{
	// Resize Swapchain if needed:
	VkExtent2D windowExtent = context->window->Extent();
	VkExtent2D surfaceExtend = context->surface->CurrentExtent();
	if (rebuildSwapchain || windowExtent.width != surfaceExtend.width || windowExtent.height != surfaceExtend.height)
	{
		rebuildSwapchain = false;
		context->ResetFrameIndex();
		RebuildSwapchain();
	}

	// Wait for fence of previous frame with same frameIndex to finish:
	VKA(vkWaitForFences(context->LogicalDevice(), 1, &fences[context->frameIndex], VK_TRUE, UINT64_MAX));
	if (!AcquireImage() || scene->activeCamera == nullptr)
		return 0;
	VKA(vkResetFences(context->LogicalDevice(), 1, &fences[context->frameIndex]));

	RecordShadowCommandBuffer(scene);
	RecordShadingCommandBuffer(scene);
	SubmitCommandBuffers();
	if (!PresentImage())
		return 0;

	return 1;
}



// Private:
void VulkanRenderer::RebuildSwapchain()
{
	// Wait for graphicsQueue to finish:
	VKA(vkQueueWaitIdle(context->logicalDevice->graphicsQueue.queue));

	// Recreate swapchain:
	std::unique_ptr<VulkanSwapchain> newSwapchain = std::make_unique<VulkanSwapchain>(context->logicalDevice.get(), context->surface.get(), VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, context->swapchain.get());
	context->swapchain.swap(newSwapchain);

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
	VkResult result = vkAcquireNextImageKHR(context->LogicalDevice(), context->Swapchain(), UINT64_MAX, acquireSemaphores[context->frameIndex], VK_NULL_HANDLE, &imageIndex);

	// Resize if needed:
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || context->window->framebufferResized)
	{
		context->window->framebufferResized = false;
		rebuildSwapchain = true;
		return false;
	}
	else
	{
		VKA(result);
		return true;
	}
}

void VulkanRenderer::RecordShadowCommandBuffer(Scene* scene)
{
	vkResetCommandPool(context->LogicalDevice(), shadowCommands[context->frameIndex].pool, 0);
	VkCommandBuffer commandBuffer = shadowCommands[context->frameIndex].buffer;

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
		renderPassBeginInfo.framebuffer = renderPass->framebuffers[context->frameIndex];
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
			for (auto& light : scene->directionalLights)
			{
				if (light == nullptr)
					continue;

				for (auto& meshRenderer : scene->GetSortedMeshRenderers())
				{
					if (meshRenderer->IsActive() && meshRenderer->castShadows)
					{
						// Update shader specific data (push constants and uniform buffers):
						Float4x4 localToClipMatrix = light->GetProjectionMatrix() * light->GetViewMatrix() * meshRenderer->transform->GetLocalToWorldMatrix();
						ShadowPushConstant pushConstant(shadowMapIndex, localToClipMatrix);
						vkCmdPushConstants(commandBuffer, MeshRenderer::GetShadowPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ShadowPushConstant), &pushConstant);
						meshRenderer->shadowMaterialProperties->UpdateShaderData();

						const VkDeviceSize offsets[1] = { 0 };
						vkCmdBindVertexBuffers(commandBuffer, 0, 1, &meshRenderer->mesh->GetVertexBuffer(context)->buffer, offsets);
						vkCmdBindIndexBuffer(commandBuffer, meshRenderer->mesh->GetIndexBuffer(context)->buffer, 0, Mesh::GetIndexType());

						vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, meshRenderer->GetShadowPipelineLayout(), 0, 1, meshRenderer->GetShadowDescriptorSets(context->frameIndex), 0, nullptr);
						vkCmdDrawIndexed(commandBuffer, 3 * meshRenderer->mesh->GetTriangleCount(), 1, 0, 0, 0);
					}
				}
				shadowMapIndex++;
			}

			// Spot Lights:
			for (auto& light : scene->spotLights)
			{
				if (light == nullptr)
					continue;

				for (auto& meshRenderer : scene->GetSortedMeshRenderers())
				{
					if (meshRenderer->IsActive() && meshRenderer->castShadows)
					{
						// Update shader specific data (push constants and uniform buffers):
						Float4x4 localToClipMatrix = light->GetProjectionMatrix() * light->GetViewMatrix() * meshRenderer->transform->GetLocalToWorldMatrix();
						ShadowPushConstant pushConstant(shadowMapIndex, localToClipMatrix);
						vkCmdPushConstants(commandBuffer, MeshRenderer::GetShadowPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ShadowPushConstant), &pushConstant);
						meshRenderer->shadowMaterialProperties->UpdateShaderData();

						const VkDeviceSize offsets[1] = { 0 };
						vkCmdBindVertexBuffers(commandBuffer, 0, 1, &meshRenderer->mesh->GetVertexBuffer(context)->buffer, offsets);
						vkCmdBindIndexBuffer(commandBuffer, meshRenderer->mesh->GetIndexBuffer(context)->buffer, 0, Mesh::GetIndexType());

						vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, meshRenderer->GetShadowPipelineLayout(), 0, 1, meshRenderer->GetShadowDescriptorSets(context->frameIndex), 0, nullptr);
						vkCmdDrawIndexed(commandBuffer, 3 * meshRenderer->mesh->GetTriangleCount(), 1, 0, 0, 0);
					}
				}
				shadowMapIndex++;
			}

			// Point Lights:
			for (auto& light : scene->pointLights)
			{
				if (light == nullptr)
					continue;

				for (uint32_t faceIndex = 0; faceIndex < 6; faceIndex++)
				{
					for (auto& meshRenderer : scene->GetSortedMeshRenderers())
					{
						if (meshRenderer->IsActive() && meshRenderer->castShadows)
						{
							// Update shader specific data (push constants and uniform buffers):
							Float4x4 localToClipMatrix = light->GetProjectionMatrix() * light->GetViewMatrix(faceIndex) * meshRenderer->transform->GetLocalToWorldMatrix();
							ShadowPushConstant pushConstant(shadowMapIndex, localToClipMatrix);
							vkCmdPushConstants(commandBuffer, MeshRenderer::GetShadowPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ShadowPushConstant), &pushConstant);
							meshRenderer->shadowMaterialProperties->UpdateShaderData();

							const VkDeviceSize offsets[1] = { 0 };
							vkCmdBindVertexBuffers(commandBuffer, 0, 1, &meshRenderer->mesh->GetVertexBuffer(context)->buffer, offsets);
							vkCmdBindIndexBuffer(commandBuffer, meshRenderer->mesh->GetIndexBuffer(context)->buffer, 0, Mesh::GetIndexType());

							vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, meshRenderer->GetShadowPipelineLayout(), 0, 1, meshRenderer->GetShadowDescriptorSets(context->frameIndex), 0, nullptr);
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
void VulkanRenderer::RecordShadingCommandBuffer(Scene* scene)
{
	vkResetCommandPool(context->LogicalDevice(), shadingCommands[context->frameIndex].pool, 0);
	VkCommandBuffer commandBuffer = shadingCommands[context->frameIndex].buffer;

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
		renderPassBeginInfo.framebuffer = renderPass->framebuffers[imageIndex];
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = context->surface->CurrentExtent();
		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassBeginInfo.pClearValues = clearValues.data();

		// Begin render pass:
		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		{
			Float3 cameraPosition = scene->activeCamera->transform->GetPosition();
			ShadingPushConstant pushConstant(Timer::GetTime(), Timer::GetDeltaTime(), scene->dLightsCount, scene->sLightsCount, scene->pLightsCount, cameraPosition);

			Material* previousMaterial = nullptr;
			for (auto& meshRenderer : scene->GetSortedMeshRenderers())
			{
				if (meshRenderer->IsActive())
				{
					meshRenderer->SetShadingRenderMatrizes(scene->activeCamera);
					meshRenderer->SetShadingLightData(scene->directionalLights);
					meshRenderer->SetShadingLightData(scene->spotLights);
					meshRenderer->SetShadingLightData(scene->pointLights);
					meshRenderer->shadingMaterialProperties->UpdateShaderData();

					if (previousMaterial != meshRenderer->GetShadingMaterial())
					{
						previousMaterial = meshRenderer->GetShadingMaterial();
						vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, meshRenderer->GetShadingPipeline());
						vkCmdPushConstants(commandBuffer, meshRenderer->GetShadingPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(ShadingPushConstant), &pushConstant);
					}

					vkCmdBindVertexBuffers(commandBuffer, 0, meshRenderer->mesh->GetBindingCount(), meshRenderer->mesh->GetBuffers(context), meshRenderer->mesh->GetOffsets());
					vkCmdBindIndexBuffer(commandBuffer, meshRenderer->mesh->GetIndexBuffer(context)->buffer, 0, Mesh::GetIndexType());

					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, meshRenderer->GetShadingPipelineLayout(), 0, 1, meshRenderer->GetShadingDescriptorSets(context->frameIndex), 0, nullptr);
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
		submitInfo.pWaitSemaphores = &acquireSemaphores[context->frameIndex];			// wait for acquireSemaphor
		submitInfo.pWaitDstStageMask = &waitStage;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &shadowCommands[context->frameIndex].buffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &shadowToShadingSemaphores[context->frameIndex]; // signal shadowToShadingSemaphore when done
		VKA(vkQueueSubmit(context->logicalDevice->graphicsQueue.queue, 1, &submitInfo, nullptr)); // signal fence when done
	}

	// Shading render pass:
	{
		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;			// wait at fragment shader stage
		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &shadowToShadingSemaphores[context->frameIndex];	// wait for shadowToShadingSemaphore
		submitInfo.pWaitDstStageMask = &waitStage;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &shadingCommands[context->frameIndex].buffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &releaseSemaphores[context->frameIndex];			// signal releaseSemaphor when done
		VKA(vkQueueSubmit(context->logicalDevice->graphicsQueue.queue, 1, &submitInfo, fences[context->frameIndex])); // signal fence when done
	}
}

bool VulkanRenderer::PresentImage()
{
	VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &releaseSemaphores[context->frameIndex]; // wait for releaseSemaphor
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &context->Swapchain();
	presentInfo.pImageIndices = &imageIndex;

	VkResult result = vkQueuePresentKHR(context->logicalDevice->presentQueue.queue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		rebuildSwapchain = true;
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
	viewport.width = (float)context->surface->CurrentExtent().width;
	viewport.height = (float)context->surface->CurrentExtent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.extent = context->surface->CurrentExtent();

	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void VulkanRenderer::CreateFences()
{
	VkFenceCreateInfo createInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;	// Fence is created in the signaled state to prevent the first wait from blocking.

	fences.resize(context->framesInFlight);
	for (uint32_t i = 0; i < context->framesInFlight; i++)
		VKA(vkCreateFence(context->LogicalDevice(), &createInfo, nullptr, &fences[i]));
}

void VulkanRenderer::CreateSemaphores()
{
	VkSemaphoreCreateInfo createInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

	acquireSemaphores.resize(context->framesInFlight);
	shadowToShadingSemaphores.resize(context->framesInFlight);
	releaseSemaphores.resize(context->framesInFlight);
	for (uint32_t i = 0; i < context->framesInFlight; i++)
	{
		VKA(vkCreateSemaphore(context->LogicalDevice(), &createInfo, nullptr, &acquireSemaphores[i]));
		VKA(vkCreateSemaphore(context->LogicalDevice(), &createInfo, nullptr, &shadowToShadingSemaphores[i]));
		VKA(vkCreateSemaphore(context->LogicalDevice(), &createInfo, nullptr, &releaseSemaphores[i]));
	}
}

void VulkanRenderer::DestroyFences()
{
	for (uint32_t i = 0; i < context->framesInFlight; i++)
		vkDestroyFence(context->LogicalDevice(), fences[i], nullptr);
	fences.clear();
}

void VulkanRenderer::DestroySemaphores()
{
	for (uint32_t i = 0; i < context->framesInFlight; i++)
	{
		vkDestroySemaphore(context->LogicalDevice(), acquireSemaphores[i], nullptr);
		vkDestroySemaphore(context->LogicalDevice(), shadowToShadingSemaphores[i], nullptr);
		vkDestroySemaphore(context->LogicalDevice(), releaseSemaphores[i], nullptr);
	}
	acquireSemaphores.clear();
	shadowToShadingSemaphores.clear();
	releaseSemaphores.clear();
}