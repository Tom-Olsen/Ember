#include <iostream>
#include "application.h"
#include "macros.h"
#include "logger.h"



// Constructor:
Application::Application()
{
	window = std::make_unique<SdlWindow>();

	// Get instance extensions:
	std::vector<const char*> instanceExtensions;
	instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	instanceExtensions.push_back(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);
	window->AddSdlInstanceExtensions(instanceExtensions);	// sdl instance extensions
	// and more ...

	// Get device extensions:
	std::vector<const char*> deviceExtensions;
	deviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	// and more ...

	// Create unique pointers:
	instance = std::make_unique<VulkanInstance>(instanceExtensions);
	physicalDevice = std::make_unique<VulkanPhysicalDevice>(instance.get());
	surface = std::make_unique<VulkanSurface>(instance.get(), physicalDevice.get(), window.get());
	logicalDevice = std::make_unique<VulkanLogicalDevice>(physicalDevice.get(), surface.get(), deviceExtensions);
	swapchain = std::make_unique<VulkanSwapchain>(window.get(), logicalDevice.get(), surface.get(), VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
	renderpass = std::make_unique<VulkanRenderpass>(logicalDevice.get(), surface->surfaceFormat.format);
	pipeline = std::make_unique<VulkanPipeline>(logicalDevice.get(), renderpass.get(), "../shaders/triangleVert.spv", "../shaders/triangleFrag.spv");
	frameBuffers = std::make_unique<VulkanFrameBuffers>(logicalDevice.get(), surface.get(), swapchain.get(), renderpass.get());
	commands = std::make_unique<VulkanCommands>(framesInFlight, logicalDevice.get());

	// Initialize mesh:
	mesh = std::make_unique<Mesh>();
	std::vector<Float3> positions;
	positions.emplace_back(-0.5f, -0.5f, 0.0f);
	positions.emplace_back(-0.5f, 0.5f, 0.0f);
	positions.emplace_back(0.5f, -0.5f, 0.0f);
	positions.emplace_back(0.5f, 0.5f, 0.0f);
	std::vector<Float4> colors;
	colors.emplace_back(1.0f, 0.0f, 0.0f, 1.0f);
	colors.emplace_back(0.0f, 1.0f, 0.0f, 1.0f);
	colors.emplace_back(0.0f, 0.0f, 1.0f, 1.0f);
	colors.emplace_back(1.0f, 1.0f, 1.0f, 1.0f);
	std::vector<Int3> triangles;
	triangles.emplace_back(0, 2, 1);
	triangles.emplace_back(1, 2, 3);
	mesh->SetPositions(std::move(positions));
	mesh->SetColors(std::move(colors));
	mesh->SetTriangles(std::move(triangles));

	// Load mesh into vertex and index buffers:
	vertexBuffer = std::make_unique<VulkanVertexBuffer>(logicalDevice.get(), physicalDevice.get(), mesh.get());
	indexBuffer = std::make_unique<VulkanIndexBuffer>(logicalDevice.get(), physicalDevice.get(), mesh.get());

	// Synchronization objects:
	CreateFences();
	CreateSemaphores();
	
	// Debug:
	//PrintApplicationStatus();
}



// Destructor:
Application::~Application()
{
	VKA(vkDeviceWaitIdle(logicalDevice->device));
	DestroyFences();
	DestroySemaphores();
}



// Main Loop:
void Application::Run()
{
	frameIndex = 0;
	int frame = 0;
	double time = 0;
	auto start = std::chrono::steady_clock::now();
	while (window->HandelEvents())
	{
		if (window->isMinimized)
			continue;
		Render();

		auto end = std::chrono::steady_clock::now();
		double deltaTime = std::chrono::duration<double>(end - start).count();
		start = end;
		time += deltaTime;
		//LOG_INFO("Frame: {}", frame);
		//LOG_INFO("Time: {}ms", 1000 * deltaTime);
		frame++;
	}
}



// Private:
void Application::PrintApplicationStatus()
{
	LOG_INFO("There are multiple extensions needed for the 'GPU-Assisted Validation' to work properly. These are automatically enabled by the validation layer. Any warnings related to this can be ignored.");
	LOG_INFO("The 'BestPractices-vkBindBufferMemory-small-dedicated-allocation' warnings can also be ignored as the current memory allocations are very small due to the nature of the tutorial this application is based on.");
	std::cout << "Press 'Enter' to continue." << std::endl;
	std::cin.get();
}

void Application::Render()
{
	// Wait for fence of previous frame with same frameIndex to finish:
	VKA(vkWaitForFences(logicalDevice->device, 1, &fences[frameIndex], VK_TRUE, UINT64_MAX));
	uint32_t imageIndex = AquireImage();
	if (imageIndex == -1)
		return;
	VKA(vkResetFences(logicalDevice->device, 1, &fences[frameIndex]));

	RecordCommandBuffer(imageIndex);
	SubmitCommandBuffer();
	PresentImage(imageIndex);

	frameIndex = (frameIndex + 1) % framesInFlight;
}

uint32_t Application::AquireImage()
{
	// Signal acquireSemaphore when done:
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(logicalDevice->device, swapchain->swapchain, UINT64_MAX, acquireSemaphores[frameIndex], VK_NULL_HANDLE, &imageIndex);

	// Resize if needed:
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		LOG_INFO("AquireImage Resize.");
		Resize();
		// reset acquire semaphor as current render pass is invalid:
		vkDestroySemaphore(logicalDevice->device, acquireSemaphores[frameIndex], nullptr);
		VkSemaphoreCreateInfo createInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
		VKA(vkCreateSemaphore(logicalDevice->device, &createInfo, nullptr, &acquireSemaphores[frameIndex]));
		return -1;
	}
	else
		VKA(result);

	return imageIndex;
}

void Application::RecordCommandBuffer(uint32_t imageIndex)
{
	// Reset command buffers of current command pool:
	//vkResetCommandBuffer(commands->buffers[frameIndex], 0); //  requires VK_COMMAND_POOL_CREATE_TRANSIENT_BIT flag in command pool creation.
	vkResetCommandPool(logicalDevice->device, commands->pools[frameIndex], 0); // requires VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT flag in command pool creation.

	// Get current command buffer:
	VkCommandBuffer commandBuffer = commands->buffers[frameIndex];

	// Begin command buffer:
	VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;		// The command buffer will be rerecorded right after executing it once.
	//beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;	// This is a secondary command buffer that will be entirely within a single render pass.
	//beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;		// The command buffer can be resubmitted while it is also already pending execution.
	//beginInfo.pInheritanceInfo = nullptr; // Needed for secondary command buffers.
	VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));

	// Viewport and Scissor:
	SetViewportAndScissor(commandBuffer);

	// Begin render pass:
	VkClearValue clearValue = { 0.0f, 0.0f, 0.0f, 1.0f };
	VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
	renderPassBeginInfo.renderPass = renderpass->renderpass;
	renderPassBeginInfo.framebuffer = (*frameBuffers)[imageIndex];
	renderPassBeginInfo.renderArea.offset = {0, 0};
	renderPassBeginInfo.renderArea.extent = surface->CurrentExtent();
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = &clearValue;

	// VK_SUBPASS_CONTENTS_INLINE = The render pass commands will be embedded in the primary command buffer itself and no secondary command buffers will be executed.
	// VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS = The render pass commands will be executed from secondary command buffers.
	vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);

	// Bind vertex buffer to command buffer:
	VkDeviceSize offsets[2] = { 0, mesh->SizeOfPositions() };
	VkBuffer buffers[2] = { vertexBuffer->buffer->buffer, vertexBuffer->buffer->buffer };
	vkCmdBindVertexBuffers(commandBuffer, 0, 2, buffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer->buffer->buffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdDrawIndexed(commandBuffer, 3 * mesh.get()->GetTriangleCount(), 1, 0, 0, 0);
	vkCmdDraw(commandBuffer, 3, 1, 0, 0);
	vkCmdEndRenderPass(commandBuffer);

	VKA(vkEndCommandBuffer(commandBuffer));
}

void Application::SubmitCommandBuffer()
{
	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // wait at color attachment stage (fragment shader).
	VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &acquireSemaphores[frameIndex];	// wait for acquireSemaphor
	submitInfo.pWaitDstStageMask = &waitStage;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commands->buffers[frameIndex];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &releaseSemaphores[frameIndex]; // signal releaseSemaphor when done
	VKA(vkQueueSubmit(logicalDevice->graphicsQueue.queue, 1, &submitInfo, fences[frameIndex])); // signal fence when done
}

void Application::PresentImage(uint32_t imageIndex)
{
	VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &releaseSemaphores[frameIndex]; // wait for releaseSemaphor
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain->swapchain;
	presentInfo.pImageIndices = &imageIndex;

	VkResult result = vkQueuePresentKHR(logicalDevice->graphicsQueue.queue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		LOG_INFO("PresentImage Resize.");
		Resize();
	}
	else
		VKA(result);
}

void Application::SetViewportAndScissor(VkCommandBuffer& commandBuffer)
{
	VkViewport viewport = {};
	viewport.width = (float)surface->CurrentExtent().width;
	viewport.height = (float)surface->CurrentExtent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.extent = surface->CurrentExtent();

	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void Application::Resize()
{
	int width = 0, height = 0;
	while (width == 0 || height == 0)
	{
		SDL_GetWindowSize(window.get()->window, &width, &height);
		SDL_WaitEvent(NULL);
	}

	// Wait for device to finish:
	VKA(vkDeviceWaitIdle(logicalDevice->device));
	
	// Recreate swapchain:
	std::unique_ptr<VulkanSwapchain> newSwapchain = std::make_unique<VulkanSwapchain>(window.get(), logicalDevice.get(), surface.get(), VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, swapchain.get());
	swapchain.swap(newSwapchain);

	// Recreate render pass and frameBuffers:
	std::unique_ptr<VulkanRenderpass> newRenderpass = std::make_unique<VulkanRenderpass>(logicalDevice.get(), surface->surfaceFormat.format);
	renderpass.swap(newRenderpass);
	frameBuffers->Recreate(swapchain.get(), renderpass.get());
}

void Application::CreateFences()
{
	VkFenceCreateInfo createInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;	// Fence is created in the signaled state to prevent the first wait from blocking.

	fences.resize(framesInFlight);
	for (uint32_t i = 0; i < framesInFlight; i++)
		VKA(vkCreateFence(logicalDevice->device, &createInfo, nullptr, &fences[i]));
}

void Application::CreateSemaphores()
{
	VkSemaphoreCreateInfo createInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

	acquireSemaphores.resize(framesInFlight);
	releaseSemaphores.resize(framesInFlight);
	for (uint32_t i = 0; i < framesInFlight; i++)
	{
		VKA(vkCreateSemaphore(logicalDevice->device, &createInfo, nullptr, &acquireSemaphores[i]));
		VKA(vkCreateSemaphore(logicalDevice->device, &createInfo, nullptr, &releaseSemaphores[i]));
	}
}

void Application::DestroyFences()
{
	for (uint32_t i = 0; i < framesInFlight; i++)
		vkDestroyFence(logicalDevice->device, fences[i], nullptr);
	fences.clear();
}

void Application::DestroySemaphores()
{
	for (uint32_t i = 0; i < framesInFlight; i++)
	{
		vkDestroySemaphore(logicalDevice->device, acquireSemaphores[i], nullptr);
		vkDestroySemaphore(logicalDevice->device, releaseSemaphores[i], nullptr);
	}
	acquireSemaphores.clear();
	releaseSemaphores.clear();
}