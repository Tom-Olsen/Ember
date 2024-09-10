#ifndef __INCLUDE_GUARD_vulkanSwapchain_h__
#define __INCLUDE_GUARD_vulkanSwapchain_h__
#include <vector>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <memory>
#include "sdlWindow.h"
#include "vulkanLogicalDevice.h"
#include "vulkanSurface.h"



/// <summary>
/// VulkanSwapchain class owns the swapchain and its images and image views.
/// Allows for simple recreation of the swapchain by passing the old swapchain in the constructor.
/// </summary>
class VulkanSwapchain
{
public: // Members:
	VkSwapchainKHR swapchain;
	std::vector<VkImage> images;
	std::vector<VkImageView> imageViews;

private: // Members:
	SdlWindow* window;
	VulkanLogicalDevice* logicalDevice;
	VulkanSurface* surface;

public: // Methods:
	VulkanSwapchain(SdlWindow* window, VulkanLogicalDevice* logicalDevice, VulkanSurface* surface, VkImageUsageFlags usage, VulkanSwapchain* oldSwapchain = nullptr);
	~VulkanSwapchain();

private: // Methods:
	void CreateSwapchain(VkImageUsageFlags usage, VulkanSwapchain* oldSwapchain);
	void CreateImages();
	void CreateImageViews();
};



#endif // __INCLUDE_GUARD_vulkanSwapchain_h__