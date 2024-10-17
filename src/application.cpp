#include <iostream>
#include "application.h"
#include "macros.h"
#include "logger.h"



// TODO:
// - rename uniformObject to constant ???
// - try multi mesh rendering
// - render image while resizing



// Constructor:
Application::Application()
{
	// Window:
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

	// Vulkan context:
	instance = std::make_unique<VulkanInstance>(instanceExtensions);
	physicalDevice = std::make_unique<VulkanPhysicalDevice>(instance.get());
	surface = std::make_unique<VulkanSurface>(instance.get(), physicalDevice.get(), window.get());
	logicalDevice = std::make_unique<VulkanLogicalDevice>(physicalDevice.get(), surface.get(), deviceExtensions);
	allocator = std::make_unique<VulkanMemoryAllocator>(instance.get(), logicalDevice.get(), physicalDevice.get());
	descriptorPool = std::make_unique<VulkanDescriptorPool>(logicalDevice.get());
	context = std::make_unique<VulkanContext>(window.get(), instance.get(), physicalDevice.get(), surface.get(), logicalDevice.get(), allocator.get(), descriptorPool.get(), framesInFlight);

	// Other:
	swapchain = std::make_unique<VulkanSwapchain>(context.get(), VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
	renderpass = std::make_unique<VulkanRenderpass>(context.get(), physicalDevice->maxMsaaSamples);

	// Render resources:
	msaaImage = std::make_unique<VulkanMsaaImage>(context.get());
	depthImage = std::make_unique<VulkanDepthImage>(context.get());
	frameBuffers = std::make_unique<VulkanFrameBuffers>(context.get(), swapchain.get(), renderpass.get(), depthImage.get(), msaaImage.get());
	commands.reserve(framesInFlight);
	for (uint32_t i = 0; i < framesInFlight; i++)
		commands.emplace_back(context.get(), logicalDevice->graphicsQueue);

	// Synchronization objects:
	CreateFences();
	CreateSemaphores();

	// Initialize mesh:
	mesh = std::make_unique<Mesh>(context.get());
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

	// Sampler:
	sampler = std::make_unique<VulkanSampler>(context.get(), "defaultSampler");

	// Texture:
	texture2d = std::make_unique<Texture2d>(context.get(), "../textures/example.jpg", "example");

	// Material:
	material = std::make_unique<Material>(context.get(), renderpass.get(), std::string("../shaders/vert.spv"), std::string("../shaders/frag.spv"));
	materialProperties = material->GetMaterialProperties();
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
			context->ResetFrameIndex();
			ResizeSwapchain();
		}

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
void Application::Render()
{
	// Wait for fence of previous frame with same frameIndex to finish:
	VKA(vkWaitForFences(logicalDevice->device, 1, &fences[context->frameIndex], VK_TRUE, UINT64_MAX));
	if (!AcquireImage())
		return;
	VKA(vkResetFences(logicalDevice->device, 1, &fences[context->frameIndex]));

	// Update uniform buffer:
	VkExtent2D windowExtent = window->Extent();
	uniformObject.model = glm::rotate(Float4x4(1.0f), (float)time * glm::radians(90.0f), Float3(0.0f, 0.0f, 1.0f));
	uniformObject.view = glm::lookAt(Float3(2.0f, 2.0f, 2.0f), Float3(0.0f, 0.0f, 0.0f), Float3(0.0f, 0.0f, 1.0f));
	uniformObject.proj = glm::perspective(glm::radians(45.0f), windowExtent.width / (float)windowExtent.height, 0.1f, 10.0f);
	uniformObject.proj[1][1] *= -1; // flip y-axis as it is inverted by default
	materialProperties.SetUniformBuffer("UniformBufferObject", uniformObject);
	materialProperties.SetSampler("samplerState", sampler.get());
	materialProperties.SetTexture2d("texture", texture2d.get());

	RecordCommandBuffer();
	SubmitCommandBuffer();
	if (!PresentImage())
		return;

	context->UpdateFrameIndex();
}

bool Application::AcquireImage()
{
	// Signal acquireSemaphore when done:
	VkResult result = vkAcquireNextImageKHR(logicalDevice->device, swapchain->swapchain, UINT64_MAX, acquireSemaphores[context->frameIndex], VK_NULL_HANDLE, &imageIndex);

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
	//vkResetCommandBuffer(commands[context->frameIndex].buffer, 0); //  requires VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT flag in command pool creation.
	
	// Reset entire command pool of current frame (more efficient):
	vkResetCommandPool(logicalDevice->device, commands[context->frameIndex].pool, 0); // requires no flags in command pool creation.

	// Get current command buffer:
	VkCommandBuffer commandBuffer = commands[context->frameIndex].buffer;

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
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipeline->pipeline);

	// Bind vertex buffer to command buffer (TODO: move buffers into mesh as getter):
	VkBuffer buffers[3] = { mesh->GetVertexBuffer()->buffer, mesh->GetVertexBuffer()->buffer, mesh->GetVertexBuffer()->buffer };
	vkCmdBindVertexBuffers(commandBuffer, 0, Mesh::GetBindingCount(), buffers, mesh->GetOffsets().data());
	vkCmdBindIndexBuffer(commandBuffer, mesh->GetIndexBuffer()->buffer, 0, Mesh::GetIndexType());

	// Push constants:
	VkExtent2D windowExtent = window->Extent();
	Float4x4 transform = glm::rotate(Float4x4(1.0f), (float)time * glm::radians(90.0f), Float3(0.0f, 0.0f, 1.0f));
	Float4x4 view = glm::lookAt(Float3(2.0f, 2.0f, 2.0f), Float3(0.0f, 0.0f, 0.0f), Float3(0.0f, 0.0f, 1.0f));
	Float4x4 proj = glm::perspective(glm::radians(45.0f), windowExtent.width / (float)windowExtent.height, 0.1f, 10.0f);
	proj[1][1] *= -1; // flip y-axis as it is inverted by default
	Float4x4 projView = proj * view;
	PushConstantObject transformData(transform, projView);
	vkCmdPushConstants(commandBuffer, material->pipeline->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantObject), &transformData);

	VkDescriptorSet* descriptorSet = &materialProperties.descriptorSets[context->frameIndex];
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipeline->pipelineLayout, 0, 1, descriptorSet, 0, nullptr);
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
	submitInfo.pWaitSemaphores = &acquireSemaphores[context->frameIndex];	// wait for acquireSemaphor
	submitInfo.pWaitDstStageMask = &waitStage;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commands[context->frameIndex].buffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &releaseSemaphores[context->frameIndex]; // signal releaseSemaphor when done
	VKA(vkQueueSubmit(logicalDevice->graphicsQueue.queue, 1, &submitInfo, fences[context->frameIndex])); // signal fence when done
}

bool Application::PresentImage()
{
	VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &releaseSemaphores[context->frameIndex]; // wait for releaseSemaphor
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
	std::unique_ptr<VulkanSwapchain> newSwapchain = std::make_unique<VulkanSwapchain>(context.get(), VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, swapchain.get());
	swapchain.swap(newSwapchain);

	// Recreate depth image:
	std::unique_ptr<VulkanDepthImage> newDepthImage = std::make_unique<VulkanDepthImage>(context.get());
	depthImage.swap(newDepthImage);

	// Recreate msaa image:
	std::unique_ptr<VulkanMsaaImage> newMsaaImage = std::make_unique<VulkanMsaaImage>(context.get());
	msaaImage.swap(newMsaaImage);

	// Recreate renderpass:
	std::unique_ptr<VulkanRenderpass> newRenderpass = std::make_unique<VulkanRenderpass>(context.get(), physicalDevice->maxMsaaSamples);
	renderpass.swap(newRenderpass);

	// Recreate frameBuffers:
	std::unique_ptr<VulkanFrameBuffers> newFrameBuffers = std::make_unique<VulkanFrameBuffers>(context.get(), swapchain.get(), renderpass.get(), depthImage.get(), msaaImage.get());
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