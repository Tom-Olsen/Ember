#ifndef __INCLUDE_GUARD_vulkanSwapchain_h__
#define __INCLUDE_GUARD_vulkanSwapchain_h__
#include <vector>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <memory>
#include "sdlWindow.h"
#include "vulkanContext.h"
#include "vulkanSurface.h"



class VulkanSwapchain
{
private:
	// external:
	SdlWindow* window;
	VulkanContext* context;
	VulkanSurface* surface;

public:
	// internal:
	VkSwapchainKHR swapchain;
	std::vector<VkImage> images;
	std::vector<VkImageView> imageViews;

	// Constructor:
	VulkanSwapchain(SdlWindow* window, VulkanContext* context, VulkanSurface* surface, VkImageUsageFlags usage, VulkanSwapchain* oldSwapchain = nullptr);

	// Destructor:
	~VulkanSwapchain();

private:
	void CreateSwapchain(VkImageUsageFlags usage, VulkanSwapchain* oldSwapchain);
	void CreateImages();
	void CreateImageViews();
};
#endif // __INCLUDE_GUARD_vulkanSwapchain_h__