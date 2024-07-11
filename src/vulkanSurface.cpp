#include "vulkanSurface.h"
#include <SDL3/SDL_vulkan.h>
#include "macros.h"



// Constructor:
VulkanSurface::VulkanSurface(VulkanContext* context, SdlWindow* window)
{
	this->context = context;
	this->window = window;

	CreateSurface();
	CreateSurfaceFormat();
}



// Destructor:
VulkanSurface::~VulkanSurface()
{
	vkDestroySurfaceKHR(context->instance, surface, nullptr);
}



// Internal:
void VulkanSurface::CreateSurface()
{
	SDL_Vulkan_CreateSurface(window->window, context->instance, nullptr, &surface);
	VkBool32 supportsSurface = false;
	VKA(vkGetPhysicalDeviceSurfaceSupportKHR(context->physicalDevice, context->graphicsQueue.familyIndex, surface, &supportsSurface));
	if (!supportsSurface)
		throw std::runtime_error("Graphics queue does not support surface!");
}

void VulkanSurface::CreateSurfaceFormat()
{
	// Get surface Format:
	uint32_t surfaceFormatCount;
	VKA(vkGetPhysicalDeviceSurfaceFormatsKHR(context->physicalDevice, surface, &surfaceFormatCount, nullptr));
	std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
	VKA(vkGetPhysicalDeviceSurfaceFormatsKHR(context->physicalDevice, surface, &surfaceFormatCount, surfaceFormats.data()));
	if (surfaceFormatCount <= 0)
		LOG_CRITICAL("No surface formats found!");

	// First available surface format should be a sensible default in most cases:
	surfaceFormat = surfaceFormats[0];
}