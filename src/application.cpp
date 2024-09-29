#include <iostream>
#include "application.h"
#include "macros.h"
#include "logger.h"



// TODO:
// - render image while resizing



// Constructor:
Application::Application()
{
	window = std::make_unique<SdlWindow>();

	// Get instance extensions:
	std::vector<const char*> instanceExtensions;
	#if defined(VALIDATION_LAYERS_ACTIVE)
	instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	instanceExtensions.push_back(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);
	#endif
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
	msaaImage = std::make_unique<VulkanMsaaImage>(logicalDevice.get(), physicalDevice.get(), surface.get());
	depthImage = std::make_unique<VulkanDepthImage>(logicalDevice.get(), physicalDevice.get(), surface.get());
	renderpass = std::make_unique<VulkanRenderpass>(logicalDevice.get(), surface->surfaceFormat.format, physicalDevice->maxMsaaSamples);
	pipelineLayout = std::make_unique<VulkanPipelineLayout>(logicalDevice.get());
	pipeline = std::make_unique<VulkanPipeline>(logicalDevice.get(), physicalDevice.get(), pipelineLayout.get(), renderpass.get(), "../shaders/triangleVert.spv", "../shaders/triangleFrag.spv");
	frameBuffers = std::make_unique<VulkanFrameBuffers>(logicalDevice.get(), surface.get(), swapchain.get(), renderpass.get(), depthImage.get(), msaaImage.get());
	commands = std::make_unique<VulkanCommands>(framesInFlight, logicalDevice.get(), logicalDevice->graphicsQueue);

	// Initialize mesh:
	mesh = std::make_unique<Mesh>();
	std::vector<Float3> positions;
	positions.emplace_back(-0.5f, -0.5f, 0.0f);
	positions.emplace_back(-0.5f, 0.5f, 0.0f);
	positions.emplace_back(0.5f, -0.5f, 0.0f);
	positions.emplace_back(0.5f, 0.5f, 0.0f);
	positions.emplace_back(-0.5f, -0.5f, -0.5f);
	positions.emplace_back(-0.5f, 0.5f, -0.5f);
	positions.emplace_back(0.5f, -0.5f, -0.5f);
	positions.emplace_back(0.5f, 0.5f, -0.5f);
	std::vector<Float4> colors;
	colors.emplace_back(1.0f, 0.0f, 0.0f, 1.0f);
	colors.emplace_back(0.0f, 1.0f, 0.0f, 1.0f);
	colors.emplace_back(0.0f, 0.0f, 1.0f, 1.0f);
	colors.emplace_back(1.0f, 1.0f, 1.0f, 1.0f);
	colors.emplace_back(1.0f, 0.0f, 0.0f, 1.0f);
	colors.emplace_back(0.0f, 1.0f, 0.0f, 1.0f);
	colors.emplace_back(0.0f, 0.0f, 1.0f, 1.0f);
	colors.emplace_back(1.0f, 1.0f, 1.0f, 1.0f);
	std::vector<Float4> uvs;
	uvs.emplace_back(0.0f, 0.0f, 0.0f, 0.0f);
	uvs.emplace_back(0.0f, 1.0f, 0.0f, 0.0f);
	uvs.emplace_back(1.0f, 0.0f, 0.0f, 0.0f);
	uvs.emplace_back(1.0f, 1.0f, 0.0f, 0.0f);
	uvs.emplace_back(0.0f, 0.0f, 0.0f, 0.0f);
	uvs.emplace_back(0.0f, 1.0f, 0.0f, 0.0f);
	uvs.emplace_back(1.0f, 0.0f, 0.0f, 0.0f);
	uvs.emplace_back(1.0f, 1.0f, 0.0f, 0.0f);
	std::vector<Int3> triangles;
	triangles.emplace_back(0, 1, 2);
	triangles.emplace_back(1, 3, 2);
	triangles.emplace_back(4, 5, 6);
	triangles.emplace_back(5, 7, 6);
	mesh->SetPositions(std::move(positions));
	mesh->SetColors(std::move(colors));
	mesh->SetUVs(std::move(uvs));
	mesh->SetTriangles(std::move(triangles));

	// Load example texture:
	sampler = std::make_unique<VulkanSampler>(logicalDevice.get(), physicalDevice.get());
	texture2d = std::make_unique<Texture2d>(logicalDevice.get(), physicalDevice.get(), sampler.get(), "../textures/example.jpg");

	// Load mesh into vertex and index buffers:
	vertexBuffer = std::make_unique<VulkanVertexBuffer>(logicalDevice.get(), physicalDevice.get(), mesh.get());
	indexBuffer = std::make_unique<VulkanIndexBuffer>(logicalDevice.get(), physicalDevice.get(), mesh.get());

	// Uniform data:
	for (size_t i = 0; i < framesInFlight; i++)
		uniformBuffers.emplace_back(logicalDevice.get(), physicalDevice.get(), sizeof(GlobalUniformObject));
	descriptorPool = std::make_unique<VulkanDescriptorPool>(logicalDevice.get(), pipelineLayout.get(), uniformBuffers, texture2d.get(), framesInFlight);

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
	time = 0;
	frameIndex = 0;
	rebuildSwapchain = false;
	auto start = std::chrono::steady_clock::now();

	bool running = true;
	while (running)
	{
		// Handle events:
		running = window->HandelEvents();

		// If window is minimized or width/height is zero, delay loop to reduce CPU usage:
		VkExtent2D windowExtent = window->Extent();
		VkExtent2D surfaceExtend = surface->CurrentExtent();
		if (window->isMinimized || windowExtent.width == 0 || windowExtent.height == 0 || surfaceExtend.width == 0 || surfaceExtend.height == 0)
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
			frameIndex = 0;
			ResizeSwapchain();
		}

		// Update uniform buffer:
		gloabalUbo.model = glm::rotate(Float4x4(1.0f), (float)time * glm::radians(90.0f), Float3(0.0f, 0.0f, 1.0f));
		gloabalUbo.view = glm::lookAt(Float3(2.0f, 2.0f, 2.0f), Float3(0.0f, 0.0f, 0.0f), Float3(0.0f, 0.0f, 1.0f));
		gloabalUbo.proj = glm::perspective(glm::radians(45.0f), windowExtent.width / (float)windowExtent.height, 0.1f, 10.0f);
		gloabalUbo.proj[1][1] *= -1; // flip y-axis as it is inverted by default
		uniformBuffers[frameIndex].UpdateBuffer(gloabalUbo);

		// Render next frame:
		Render();

		// FPS calculations:
		auto end = std::chrono::steady_clock::now();
		double deltaTime = std::chrono::duration<double>(end - start).count();
		start = end;
		time += deltaTime;
		//LOG_INFO("Time: {}ms", 1000 * deltaTime);
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
	if (!AcquireImage())
		return;
	VKA(vkResetFences(logicalDevice->device, 1, &fences[frameIndex]));

	RecordCommandBuffer();
	SubmitCommandBuffer();
	if (!PresentImage())
		return;

	frameIndex = (frameIndex + 1) % framesInFlight;
}

bool Application::AcquireImage()
{
	// Signal acquireSemaphore when done:
	VkResult result = vkAcquireNextImageKHR(logicalDevice->device, swapchain->swapchain, UINT64_MAX, acquireSemaphores[frameIndex], VK_NULL_HANDLE, &imageIndex);

	// Resize if needed:
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.get()->framebufferResized)
	{
		window.get()->framebufferResized = false;
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
	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };
	VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
	renderPassBeginInfo.renderPass = renderpass->renderpass;
	renderPassBeginInfo.framebuffer = (*frameBuffers)[imageIndex];
	renderPassBeginInfo.renderArea.offset = {0, 0};
	renderPassBeginInfo.renderArea.extent = surface->CurrentExtent();
	renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassBeginInfo.pClearValues = clearValues.data();

	// VK_SUBPASS_CONTENTS_INLINE = The render pass commands will be embedded in the primary command buffer itself and no secondary command buffers will be executed.
	// VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS = The render pass commands will be executed from secondary command buffers.
	vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);

	// Bind vertex buffer to command buffer (TODO: move buffers into mesh as getter):
	VkBuffer buffers[3] = { vertexBuffer->buffer->buffer, vertexBuffer->buffer->buffer, vertexBuffer->buffer->buffer };
	vkCmdBindVertexBuffers(commandBuffer, 0, Mesh::GetBindingCount(), buffers, mesh->GetOffsets().data());
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer->buffer->buffer, 0, Mesh::GetIndexType());

	VkDescriptorSet* descriptorSet = &descriptorPool->descriptorSets[frameIndex];
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout->pipelineLayout, 0, 1, descriptorSet, 0, nullptr);
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

bool Application::PresentImage()
{
	VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &releaseSemaphores[frameIndex]; // wait for releaseSemaphor
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain->swapchain;
	presentInfo.pImageIndices = &imageIndex;

	VkResult result = vkQueuePresentKHR(logicalDevice->presentQueue.queue, &presentInfo);

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
	viewport.width = (float)surface->CurrentExtent().width;
	viewport.height = (float)surface->CurrentExtent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.extent = surface->CurrentExtent();

	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void Application::ResizeSwapchain()
{
	// Wait for graphicsQueue to finish:
	VKA(vkQueueWaitIdle(logicalDevice->graphicsQueue.queue));
	
	// Recreate swapchain:
	std::unique_ptr<VulkanSwapchain> newSwapchain = std::make_unique<VulkanSwapchain>(window.get(), logicalDevice.get(), surface.get(), VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, swapchain.get());
	swapchain.swap(newSwapchain);

	// Recreate depth image:
	std::unique_ptr<VulkanDepthImage> newDepthImage = std::make_unique<VulkanDepthImage>(logicalDevice.get(), physicalDevice.get(), surface.get());
	depthImage.swap(newDepthImage);

	// Recreate msaa image:
	std::unique_ptr<VulkanMsaaImage> newMsaaImage = std::make_unique<VulkanMsaaImage>(logicalDevice.get(), physicalDevice.get(), surface.get());
	msaaImage.swap(newMsaaImage);

	// Recreate renderpass:
	std::unique_ptr<VulkanRenderpass> newRenderpass = std::make_unique<VulkanRenderpass>(logicalDevice.get(), surface->surfaceFormat.format, physicalDevice->maxMsaaSamples);
	renderpass.swap(newRenderpass);

	// Recreate frameBuffers:
	std::unique_ptr<VulkanFrameBuffers> newFrameBuffers = std::make_unique<VulkanFrameBuffers>(logicalDevice.get(), surface.get(), swapchain.get(), renderpass.get(), depthImage.get(), msaaImage.get());
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