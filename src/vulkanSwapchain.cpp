#include "vulkanSwapchain.h"
#include <vector>
#include "macros.h"
#include "logger.h"



// Constructor:
VulkanSwapchain::VulkanSwapchain(SdlWindow* window, VulkanContext* context, VulkanSurface* surface, VkImageUsageFlags usage, VulkanSwapchain* oldSwapchain)
{
	this->window = window;
	this->context = context;
	this->surface = surface;

	CreateSwapchain(usage, oldSwapchain);
	CreateImages();
	CreateImageViews();
}



// Destructor:
VulkanSwapchain::~VulkanSwapchain()
{
	VKA(vkDeviceWaitIdle(context->device));
	for (uint32_t i = 0; i < imageViews.size(); i++)
		vkDestroyImageView(context->device, imageViews[i], nullptr);
	vkDestroySwapchainKHR(context->device, swapchain, nullptr);
}



// Internal:
void VulkanSwapchain::CreateSwapchain(VkImageUsageFlags usage, VulkanSwapchain* oldSwapchain)
{
	if (oldSwapchain)
		surface = oldSwapchain->surface;

	VkSwapchainCreateInfoKHR createInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	createInfo.surface = surface->surface;
	createInfo.minImageCount = 3;
	createInfo.imageFormat = surface->surfaceFormat.format;
	createInfo.imageColorSpace = surface->surfaceFormat.colorSpace;
	createInfo.imageExtent = window->extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = usage;
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	createInfo.oldSwapchain = oldSwapchain ? oldSwapchain->swapchain : VK_NULL_HANDLE;
	VKA(vkCreateSwapchainKHR(context->device, &createInfo, nullptr, &swapchain));
}

void VulkanSwapchain::CreateImages()
{
	uint32_t imageCount;
	VKA(vkGetSwapchainImagesKHR(context->device, swapchain, &imageCount, nullptr));
	images.resize(imageCount);
	VKA(vkGetSwapchainImagesKHR(context->device, swapchain, &imageCount, images.data()));
}

void VulkanSwapchain::CreateImageViews()
{
	imageViews.resize(images.size());
	for (uint32_t i = 0; i < images.size(); i++)
	{
		VkImageViewCreateInfo createInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		createInfo.image = images[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = surface->surfaceFormat.format;
		createInfo.components = {};	// swizzle components (empty = 4x VK_COMPONENT_SWIZZLE_IDENTITY)
		createInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		VKA(vkCreateImageView(context->device, &createInfo, nullptr, &imageViews[i]))
	}
}