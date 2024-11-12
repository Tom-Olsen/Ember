#include "vulkanRenderer.h"
#include "vulkanMacros.h"
#include "vulkanPushConstant.h"
#include "renderPassManager.h"



// Constructor:
VulkanRenderer::VulkanRenderer(VulkanContext* context)
{
	this->context = context;

	// Command buffers:
	shadowCommands.reserve(context->framesInFlight);
	forwardCommands.reserve(context->framesInFlight);
	for (uint32_t i = 0; i < context->framesInFlight; i++)
	{
		shadowCommands.emplace_back(context, context->logicalDevice->graphicsQueue);
		forwardCommands.emplace_back(context, context->logicalDevice->graphicsQueue);
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
void VulkanRenderer::Render(Scene* scene)
{
	// Resize Swapchain if needed:
	VkExtent2D windowExtent = context->window->Extent();
	VkExtent2D surfaceExtend = context->surface->CurrentExtent();
	if (rebuildSwapchain || windowExtent.width != surfaceExtend.width || windowExtent.height != surfaceExtend.height)
	{
		rebuildSwapchain = false;
		context->ResetFrameIndex();
		ResizeSwapchain();
	}

	// Wait for fence of previous frame with same frameIndex to finish:
	VKA(vkWaitForFences(context->LogicalDevice(), 1, &fences[context->frameIndex], VK_TRUE, UINT64_MAX));
	if (!AcquireImage() || scene->activeCamera == nullptr)
		return;
	VKA(vkResetFences(context->LogicalDevice(), 1, &fences[context->frameIndex]));

	RecordShadowCommandBuffer(scene);
	RecordForwardCommandBuffer(scene);
	SubmitCommandBuffers();
	if (!PresentImage())
		return;

	context->UpdateFrameIndex();
}



// Private:
void VulkanRenderer::ResizeSwapchain()
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
		renderPassBeginInfo.framebuffer = renderPass->framebuffers[imageIndex];
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = VkExtent2D{ ShadowRenderPass::shadowMapWidth, ShadowRenderPass::shadowMapHeight };
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearValues;

		// Begin render pass:
		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		{
			// Push constants:
			VulkanPushConstant pushTime(Timer::GetTime4(), Timer::GetDeltaTime4());
		
			for (auto& pair : scene->meshRenderers)
			{
				MeshRenderer* meshRenderer = pair.second;
				if (meshRenderer->IsActive() && meshRenderer->GetShadowMaterial() != nullptr)
				{
					if (scene->directionalLights[0] != nullptr)
					{
						meshRenderer->SetShadowRenderMatrizes(scene->directionalLights[0]->GetViewMatrix(), scene->directionalLights[0]->GetProjectionMatrix());
						meshRenderer->shadowMaterialProperties->UpdateUniformBuffers(context->frameIndex);
					}

					// TODO: move these two outside of for loop and do them for each material only once.
					vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, meshRenderer->GetShadowPipeline());
					vkCmdPushConstants(commandBuffer, meshRenderer->GetShadowPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(VulkanPushConstant), &pushTime);

					// TODO: clean this up
					const VkDeviceSize offsets[1] = { 0 };
					vkCmdBindVertexBuffers(commandBuffer, 0, 1, &meshRenderer->mesh->GetVertexBuffer(context)->buffer, offsets);
					vkCmdBindIndexBuffer(commandBuffer, meshRenderer->mesh->GetIndexBuffer(context)->buffer, 0, Mesh::GetIndexType());
		
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, meshRenderer->GetShadowPipelineLayout(), 0, 1, meshRenderer->GetShadowDescriptorSets(context->frameIndex), 0, nullptr);
					vkCmdDrawIndexed(commandBuffer, 3 * meshRenderer->mesh->GetTriangleCount(), 1, 0, 0, 0);
				}
			}
		}
		vkCmdEndRenderPass(commandBuffer);
	}
	VKA(vkEndCommandBuffer(commandBuffer));
}

void VulkanRenderer::RecordForwardCommandBuffer(Scene* scene)
{
	vkResetCommandPool(context->LogicalDevice(), forwardCommands[context->frameIndex].pool, 0);
	VkCommandBuffer commandBuffer = forwardCommands[context->frameIndex].buffer;

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
		renderPassBeginInfo.renderPass = renderPass->renderPass;
		renderPassBeginInfo.framebuffer = renderPass->framebuffers[imageIndex];
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = context->surface->CurrentExtent();
		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassBeginInfo.pClearValues = clearValues.data();

		// Begin render pass:
		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		{
			// Push constants:
			VulkanPushConstant pushTime(Timer::GetTime4(), Timer::GetDeltaTime4());

			for (auto& pair : scene->meshRenderers)
			{
				MeshRenderer* meshRenderer = pair.second;
				if (meshRenderer->IsActive())
				{
					meshRenderer->SetForwardRenderMatrizes(scene->activeCamera->GetViewMatrix(), scene->activeCamera->GetProjectionMatrix());
					if (scene->directionalLights[0] != nullptr)
						meshRenderer->SetForwardLightData(scene->directionalLights);
					meshRenderer->forwardMaterialProperties->UpdateUniformBuffers(context->frameIndex);

					// TODO: move these two outside of for loop and do them for each material only once.
					vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, meshRenderer->GetForwardPipeline());
					vkCmdPushConstants(commandBuffer, meshRenderer->GetForwardPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(VulkanPushConstant), &pushTime);

					vkCmdBindVertexBuffers(commandBuffer, 0, meshRenderer->mesh->GetBindingCount(), meshRenderer->mesh->GetBuffers(context), meshRenderer->mesh->GetOffsets());
					vkCmdBindIndexBuffer(commandBuffer, meshRenderer->mesh->GetIndexBuffer(context)->buffer, 0, Mesh::GetIndexType());

					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, meshRenderer->GetForwardPipelineLayout(), 0, 1, meshRenderer->GetForwardDescriptorSets(context->frameIndex), 0, nullptr);
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
		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // wait at color attachment stage (fragment shader).
		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &acquireSemaphores[context->frameIndex];	// wait for acquireSemaphor
		submitInfo.pWaitDstStageMask = &waitStage;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &shadowCommands[context->frameIndex].buffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &shadowToForwardSemaphores[context->frameIndex]; // signal shadowToForwardSemaphore when done
		VKA(vkQueueSubmit(context->logicalDevice->graphicsQueue.queue, 1, &submitInfo, nullptr)); // signal fence when done
	}

	// Forward render pass:
	{
		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // wait at color attachment stage (fragment shader).
		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &shadowToForwardSemaphores[context->frameIndex];	// wait for shadowToForwardSemaphore
		submitInfo.pWaitDstStageMask = &waitStage;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &forwardCommands[context->frameIndex].buffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &releaseSemaphores[context->frameIndex]; // signal releaseSemaphor when done
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
	shadowToForwardSemaphores.resize(context->framesInFlight);
	releaseSemaphores.resize(context->framesInFlight);
	for (uint32_t i = 0; i < context->framesInFlight; i++)
	{
		VKA(vkCreateSemaphore(context->LogicalDevice(), &createInfo, nullptr, &acquireSemaphores[i]));
		VKA(vkCreateSemaphore(context->LogicalDevice(), &createInfo, nullptr, &shadowToForwardSemaphores[i]));
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
		vkDestroySemaphore(context->LogicalDevice(), shadowToForwardSemaphores[i], nullptr);
		vkDestroySemaphore(context->LogicalDevice(), releaseSemaphores[i], nullptr);
	}
	acquireSemaphores.clear();
	shadowToForwardSemaphores.clear();
	releaseSemaphores.clear();
}