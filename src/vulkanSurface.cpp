#include "vulkanSurface.h"
#include <SDL3/SDL_vulkan.h>
#include <stdexcept>
#include "macros.h"



// Constructor:
VulkanSurface::VulkanSurface(VulkanInstance* instance, VulkanPhysicalDevice* physicalDevice, SdlWindow* window)
{
	this->instance = instance;
	this->physicalDevice = physicalDevice;
	this->window = window;

	// Surface:
	SDL_Vulkan_CreateSurface(window->window, instance->instance, nullptr, &surface);

	// SurfaceCapabilities:
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice->device, surface, &surfaceCapabilities);

	// Available surfaceFormats:
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice->device, surface, &formatCount, nullptr);
	availableSurfaceFormats.resize(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice->device, surface, &formatCount, availableSurfaceFormats.data());
	if (availableSurfaceFormats.empty())
		throw std::runtime_error("No surface formats found!");

	// Available presentModes:
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice->device, surface, &presentModeCount, nullptr);
	availablePresentModes.resize(presentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice->device, surface, &presentModeCount, availablePresentModes.data());
	if (availablePresentModes.empty())
		throw std::runtime_error("No present modes found!");
	
	// Pick surfaceFormat and presentMode:
	surfaceFormat = PickSurfaceFormat();
	presentMode = PickPresentMode();
}



// Destructor:
VulkanSurface::~VulkanSurface()
{
	vkDestroySurfaceKHR(instance->instance, surface, nullptr);
}



// Public:
VkExtent2D VulkanSurface::CurrentExtent()
{
	return surfaceCapabilities.currentExtent;
}
VkExtent2D VulkanSurface::MinImageExtent()
{
	return surfaceCapabilities.minImageExtent;
}
VkExtent2D VulkanSurface::MaxImageExtent()
{
	return surfaceCapabilities.maxImageExtent;
}
uint32_t VulkanSurface::MinImageCount()
{
	return surfaceCapabilities.minImageCount;
}
uint32_t VulkanSurface::MaxImageCount()
{
	return surfaceCapabilities.maxImageCount;
}



// Private:
VkSurfaceFormatKHR VulkanSurface::PickSurfaceFormat()
{
	for (const auto& format : availableSurfaceFormats)
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return format;

	// First is always the best if desired not available.
	return availableSurfaceFormats[0];
}
VkPresentModeKHR VulkanSurface::PickPresentMode()
{
	for (const VkPresentModeKHR& mode : availablePresentModes)
		if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
			return mode;

	// Always available.
	return VK_PRESENT_MODE_FIFO_KHR;
}



// Presents Modes:
// VK_PRESENT_MODE_IMMEDIATE_KHR:
// Images submitted by your application are transferred to the screen right away, which may result in tearing.

// VK_PRESENT_MODE_FIFO_KHR:
// The swap chain is a queue where the display takes an image from the front of the queue when the display is
// refreshed and the program inserts rendered images at the back of the queue.If the queue is full then the
// program has to wait.This is most similar to vertical sync as found in modern games.The moment that the
// display is refreshed is known as "vertical blank".

// VK_PRESENT_MODE_FIFO_RELAXED_KHR:
// This mode only differs from the previous one if the application is late and the queue was empty at the
// last vertical blank.Instead of waiting for the next vertical blank, the image is transferred right away
// when it finally arrives.This may result in visible tearing.

// VK_PRESENT_MODE_MAILBOX_KHR:
// This is another variation of the second mode.Instead of blocking the application when the queue is full,
// the images that are already queued are simply replaced with the newer ones.This mode can be used to render
// frames as fast as possible while still avoiding tearing, resulting in fewer latency issues than standard
// vertical sync.This is commonly known as "triple buffering", although the existence of three buffers alone
// does not necessarily mean that the framerate is unlocked.