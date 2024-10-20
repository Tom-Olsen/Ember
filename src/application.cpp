#include <iostream>
#include "application.h"
#include "macros.h"
#include "logger.h"



// TODO:
// - rename uniformObject to constant ???
// - render image while resizing



// Constructor:
Application::Application(std::shared_ptr<VulkanContext> context)
{
	this->context = context;

	// Render resources:
	msaaImage = std::make_unique<VulkanMsaaImage>(context.get());
	depthImage = std::make_unique<VulkanDepthImage>(context.get());
	frameBuffers = std::make_unique<VulkanFrameBuffers>(context.get(), depthImage.get(), msaaImage.get());
	commands.reserve(context->framesInFlight);
	for (uint32_t i = 0; i < context->framesInFlight; i++)
		commands.emplace_back(context.get(), context->logicalDevice->graphicsQueue);

	// Synchronization objects:
	CreateFences();
	CreateSemaphores();
}



// Destructor:
Application::~Application()
{
	VKA(vkDeviceWaitIdle(context->LogicalDevice()));
	DestroyFences();
	DestroySemaphores();
}



// Public methods:
void Application::Run()
{
	time = 0;
	rebuildSwapchain = false;
	auto start = std::chrono::steady_clock::now();

	bool running = true;
	while (running)
	{
		// Handle events:
		running = context->window->HandelEvents();

		// If window is minimized or width/height is zero, delay loop to reduce CPU usage:
		VkExtent2D windowExtent = context->window->Extent();
		VkExtent2D surfaceExtend = context->surface->CurrentExtent();
		if (context->window->isMinimized || windowExtent.width == 0 || windowExtent.height == 0 || surfaceExtend.width == 0 || surfaceExtend.height == 0)
		{
			SDL_Delay(10);
			continue;
		}

		// QUESTION:
		// -what is the exact difference between window and surface and how can it be that the surface extent differs from the window extent?

		// Resize Swapchain if needed:
		if (rebuildSwapchain || windowExtent.width != surfaceExtend.width || windowExtent.height != surfaceExtend.height)
		{
			rebuildSwapchain = false;
			context->ResetFrameIndex();
			ResizeSwapchain();
		}

		GameObject* cameraGO = scene->GetGameObject("camera");
		Camera* camera = cameraGO->GetComponent<Camera>();
		uniformObject.view = camera->GetViewMatrix();
		uniformObject.proj = camera->GetProjectionMatrix();
		for (auto& pair : scene->meshRenderers)
		{
			MeshRenderer* meshRenderer = pair.second;
			uniformObject.model = meshRenderer->gameObject->transform->GetLocalToWorldMatrix();
			meshRenderer->materialProperties->SetUniformBuffer("UniformBufferObject", uniformObject);
		}

		// Render next frame:
		Render();

		// FPS calculations:
		auto end = std::chrono::steady_clock::now();
		deltaTime = std::chrono::duration<double>(end - start).count();
		start = end;
		time += deltaTime;
		//LOG_INFO("Time: {}ms", 1000 * deltaTime);
	}
}
void Application::SetScene(Scene* scene)
{
	this->scene = scene;
}



// Private:
void Application::Render()
{
	// Wait for fence of previous frame with same frameIndex to finish:
	VKA(vkWaitForFences(context->LogicalDevice(), 1, &fences[context->frameIndex], VK_TRUE, UINT64_MAX));
	if (!AcquireImage())
		return;
	VKA(vkResetFences(context->LogicalDevice(), 1, &fences[context->frameIndex]));

	RecordCommandBuffer();
	SubmitCommandBuffer();
	if (!PresentImage())
		return;

	context->UpdateFrameIndex();
}

bool Application::AcquireImage()
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

void Application::RecordCommandBuffer()
{
	// Reset entire command pool of current frame (recommended):
	vkResetCommandPool(context->LogicalDevice(), commands[context->frameIndex].pool, 0);

	// Get current command buffer:
	VkCommandBuffer commandBuffer = commands[context->frameIndex].buffer;

	// Begin command buffer:
	VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
	{
		// Viewport and Scissor:
		SetViewportAndScissor(commandBuffer);

		// Render pass info:
		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
		clearValues[1].depthStencil = { 1.0f, 0 };
		VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		renderPassBeginInfo.renderPass = context->Renderpass();
		renderPassBeginInfo.framebuffer = (*frameBuffers)[imageIndex];
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = context->surface->CurrentExtent();
		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassBeginInfo.pClearValues = clearValues.data();

		// Begin render pass:
		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		{
			// Push constants:
			PushConstantObject pushTime(Float4(time, 2 * time, 3 * time, 4 * time), Float4(deltaTime, 2 * deltaTime, 3 * deltaTime, 4 * deltaTime));

			for (auto& pair : scene->meshRenderers)
			{
				MeshRenderer* meshRenderer = pair.second;

				// TODO: move these two outside of for loop and do them for each material only once.
				vkCmdPushConstants(commandBuffer, meshRenderer->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantObject), &pushTime);
				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, meshRenderer->GetPipeline());

				// TODO: clean this up
				VkBuffer buffers[3] = { meshRenderer->mesh->GetVertexBuffer(context.get())->buffer, meshRenderer->mesh->GetVertexBuffer(context.get())->buffer, meshRenderer->mesh->GetVertexBuffer(context.get())->buffer };
				vkCmdBindVertexBuffers(commandBuffer, 0, Mesh::GetBindingCount(), buffers, meshRenderer->mesh->GetOffsets().data());
				vkCmdBindIndexBuffer(commandBuffer, meshRenderer->mesh->GetIndexBuffer(context.get())->buffer, 0, Mesh::GetIndexType());

				VkDescriptorSet* descriptorSet = meshRenderer->GetDescriptorSets(context->frameIndex);
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, meshRenderer->GetPipelineLayout(), 0, 1, descriptorSet, 0, nullptr);
				vkCmdDrawIndexed(commandBuffer, 3 * meshRenderer->mesh->GetTriangleCount(), 1, 0, 0, 0);
			}
		}
		vkCmdEndRenderPass(commandBuffer);
	}
	VKA(vkEndCommandBuffer(commandBuffer));
}

void Application::SubmitCommandBuffer()
{
	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // wait at color attachment stage (fragment shader).
	VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &acquireSemaphores[context->frameIndex];	// wait for acquireSemaphor
	submitInfo.pWaitDstStageMask = &waitStage;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commands[context->frameIndex].buffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &releaseSemaphores[context->frameIndex]; // signal releaseSemaphor when done
	VKA(vkQueueSubmit(context->logicalDevice->graphicsQueue.queue, 1, &submitInfo, fences[context->frameIndex])); // signal fence when done
}

bool Application::PresentImage()
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

void Application::SetViewportAndScissor(VkCommandBuffer& commandBuffer)
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

void Application::ResizeSwapchain()
{
	// Wait for graphicsQueue to finish:
	VKA(vkQueueWaitIdle(context->logicalDevice->graphicsQueue.queue));
	
	// Recreate swapchain:
	std::unique_ptr<VulkanSwapchain> newSwapchain = std::make_unique<VulkanSwapchain>(context->logicalDevice.get(), context->surface.get(), VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, context->swapchain.get());
	context->swapchain.swap(newSwapchain);

	// Recreate renderpass:
	std::unique_ptr<VulkanRenderpass> newRenderpass = std::make_unique<VulkanRenderpass>(context->logicalDevice.get(), context->surface.get(), context->physicalDevice->maxMsaaSamples);
	context->renderpass.swap(newRenderpass);

	// Recreate msaa image:
	std::unique_ptr<VulkanMsaaImage> newMsaaImage = std::make_unique<VulkanMsaaImage>(context.get());
	msaaImage.swap(newMsaaImage);

	// Recreate depth image:
	std::unique_ptr<VulkanDepthImage> newDepthImage = std::make_unique<VulkanDepthImage>(context.get());
	depthImage.swap(newDepthImage);

	// Recreate frameBuffers:
	std::unique_ptr<VulkanFrameBuffers> newFrameBuffers = std::make_unique<VulkanFrameBuffers>(context.get(), depthImage.get(), msaaImage.get());
	frameBuffers.swap(newFrameBuffers);

	// Recreate synchronization objects:
	DestroyFences();
	DestroySemaphores();
	CreateFences();
	CreateSemaphores();
}

void Application::CreateFences()
{
	VkFenceCreateInfo createInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;	// Fence is created in the signaled state to prevent the first wait from blocking.

	fences.resize(context->framesInFlight);
	for (uint32_t i = 0; i < context->framesInFlight; i++)
		VKA(vkCreateFence(context->LogicalDevice(), &createInfo, nullptr, &fences[i]));
}

void Application::CreateSemaphores()
{
	VkSemaphoreCreateInfo createInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

	acquireSemaphores.resize(context->framesInFlight);
	releaseSemaphores.resize(context->framesInFlight);
	for (uint32_t i = 0; i < context->framesInFlight; i++)
	{
		VKA(vkCreateSemaphore(context->LogicalDevice(), &createInfo, nullptr, &acquireSemaphores[i]));
		VKA(vkCreateSemaphore(context->LogicalDevice(), &createInfo, nullptr, &releaseSemaphores[i]));
	}
}

void Application::DestroyFences()
{
	for (uint32_t i = 0; i < context->framesInFlight; i++)
		vkDestroyFence(context->LogicalDevice(), fences[i], nullptr);
	fences.clear();
}

void Application::DestroySemaphores()
{
	for (uint32_t i = 0; i < context->framesInFlight; i++)
	{
		vkDestroySemaphore(context->LogicalDevice(), acquireSemaphores[i], nullptr);
		vkDestroySemaphore(context->LogicalDevice(), releaseSemaphores[i], nullptr);
	}
	acquireSemaphores.clear();
	releaseSemaphores.clear();
}