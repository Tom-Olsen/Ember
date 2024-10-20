#include "vulkanContext.h"



// Constructor:
VulkanContext::VulkanContext(uint32_t framesInFlight)
{
	this->framesInFlight = framesInFlight;
	this->frameIndex = 0;

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

	// Create vulkan context:
	instance = std::make_unique<VulkanInstance>(instanceExtensions);
	physicalDevice = std::make_unique<VulkanPhysicalDevice>(instance.get());
	surface = std::make_unique<VulkanSurface>(instance.get(), physicalDevice.get(), window.get());
	logicalDevice = std::make_unique<VulkanLogicalDevice>(physicalDevice.get(), surface.get(), deviceExtensions);
	allocator = std::make_unique<VulkanMemoryAllocator>(instance.get(), logicalDevice.get(), physicalDevice.get());
	descriptorPool = std::make_unique<VulkanDescriptorPool>(logicalDevice.get());
	swapchain = std::make_unique<VulkanSwapchain>(logicalDevice.get(), surface.get(), VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
	renderpass = std::make_unique<VulkanRenderpass>(logicalDevice.get(), surface.get(), physicalDevice->maxMsaaSamples);
}



// Destructor:
VulkanContext::~VulkanContext()
{
	VKA(vkDeviceWaitIdle(logicalDevice->device));
}



// Getters:
SDL_Window* VulkanContext::Window()
{
	return window->window;
}
VkInstance& VulkanContext::Instance()
{
	return instance->instance;
}
VkPhysicalDevice& VulkanContext::PhysicalDevice()
{
	return physicalDevice->device;
}
VkSurfaceKHR& VulkanContext::Surface()
{
	return surface->surface;
}
VkDevice& VulkanContext::LogicalDevice()
{
	return logicalDevice->device;
}
VmaAllocator& VulkanContext::Allocator()
{
	return allocator->allocator;
}
VkDescriptorPool& VulkanContext::DescriptorPool()
{
	return descriptorPool->descriptorPool;
}
VkSwapchainKHR& VulkanContext::Swapchain()
{
	return swapchain->swapchain;
}
VkRenderPass& VulkanContext::Renderpass()
{
	return renderpass->renderpass;
}



// Public frame logic:
void VulkanContext::UpdateFrameIndex()
{
	frameIndex = (frameIndex + 1) % framesInFlight;
}
void VulkanContext::ResetFrameIndex()
{
	frameIndex = 0;
}