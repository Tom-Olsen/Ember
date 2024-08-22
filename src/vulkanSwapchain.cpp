#include "vulkanSwapchain.h"
#include <vector>
#include "macros.h"



// Constructor:
VulkanSwapchain::VulkanSwapchain(SdlWindow* window, VulkanLogicalDevice* logicalDevice, VulkanSurface* surface, VkImageUsageFlags usage, VulkanSwapchain* oldSwapchain)
{
	this->window = window;
	this->logicalDevice = logicalDevice;
	this->surface = surface;

	CreateSwapchain(usage, oldSwapchain);
	CreateImages();
	CreateImageViews();
}



// Destructor:
VulkanSwapchain::~VulkanSwapchain()
{
	VKA(vkDeviceWaitIdle(logicalDevice->device));
	for (uint32_t i = 0; i < imageViews.size(); i++)
		vkDestroyImageView(logicalDevice->device, imageViews[i], nullptr);
	vkDestroySwapchainKHR(logicalDevice->device, swapchain, nullptr);
}



// Private:
void VulkanSwapchain::CreateSwapchain(VkImageUsageFlags usage, VulkanSwapchain* oldSwapchain)
{
	if (oldSwapchain)
		surface = oldSwapchain->surface;

	uint32_t imageCount = surface->MinImageCount() + 1;
	if (surface->MaxImageCount() > 0 && imageCount > surface->MaxImageCount())
		imageCount = surface->MaxImageCount();

	VkSwapchainCreateInfoKHR createInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	createInfo.surface = surface->surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surface->surfaceFormat.format;
	createInfo.imageColorSpace = surface->surfaceFormat.colorSpace;
	createInfo.imageExtent = surface->CurrentExtent();
	createInfo.imageArrayLayers = 1;									// always 1 unless stereoscopic 3D application.
	createInfo.imageUsage = usage;
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;			// we assume that only one queue family will access the images for now.
	createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;	// dont rotate or flip.
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;		// dont blend with other windows.
	createInfo.presentMode = surface->presentMode;
	createInfo.oldSwapchain = oldSwapchain ? oldSwapchain->swapchain : VK_NULL_HANDLE;
	createInfo.clipped = VK_TRUE;										// clip pixels that are obscured by other windows.
	VKA(vkCreateSwapchainKHR(logicalDevice->device, &createInfo, nullptr, &swapchain));
}

void VulkanSwapchain::CreateImages()
{
	uint32_t imageCount;
	VKA(vkGetSwapchainImagesKHR(logicalDevice->device, swapchain, &imageCount, nullptr));
	images.resize(imageCount);
	VKA(vkGetSwapchainImagesKHR(logicalDevice->device, swapchain, &imageCount, images.data()));
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
		VKA(vkCreateImageView(logicalDevice->device, &createInfo, nullptr, &imageViews[i]))
	}
}