#include "vulkanSwapchain.h"
#include <vector>
#include "macros.h"



// Constructor:
VulkanSwapchain::VulkanSwapchain(VulkanContext* context, VkImageUsageFlags usage, VulkanSwapchain* oldSwapchain)
{
	this->context = context;

	CreateSwapchain(usage, oldSwapchain);
	CreateImages();
	CreateImageViews();
}



// Destructor:
VulkanSwapchain::~VulkanSwapchain()
{
	VKA(vkDeviceWaitIdle(context->LogicalDevice()));
	for (uint32_t i = 0; i < imageViews.size(); i++)
		vkDestroyImageView(context->LogicalDevice(), imageViews[i], nullptr);
	vkDestroySwapchainKHR(context->LogicalDevice(), swapchain, nullptr);
}



// Private:
void VulkanSwapchain::CreateSwapchain(VkImageUsageFlags usage, VulkanSwapchain* oldSwapchain)
{
	uint32_t imageCount = context->surface->MinImageCount() + 1;
	if (context->surface->MaxImageCount() > 0 && imageCount > context->surface->MaxImageCount())
		imageCount = context->surface->MaxImageCount();

	VkSwapchainCreateInfoKHR createInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	createInfo.surface = context->Surface();
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = context->surface->surfaceFormat.format;
	createInfo.imageColorSpace = context->surface->surfaceFormat.colorSpace;
	createInfo.imageExtent = context->surface->CurrentExtent();
	createInfo.imageArrayLayers = 1;									// always 1 unless stereoscopic 3D application.
	createInfo.imageUsage = usage;
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;			// we assume that only one queue family will access the images for now.
	createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;	// dont rotate or flip.
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;		// dont blend with other windows.
	createInfo.presentMode = context->surface->presentMode;
	createInfo.oldSwapchain = oldSwapchain ? oldSwapchain->swapchain : VK_NULL_HANDLE;
	createInfo.clipped = VK_TRUE;										// clip pixels that are obscured by other windows.
	VKA(vkCreateSwapchainKHR(context->LogicalDevice(), &createInfo, nullptr, &swapchain));
}

void VulkanSwapchain::CreateImages()
{
	uint32_t imageCount;
	VKA(vkGetSwapchainImagesKHR(context->LogicalDevice(), swapchain, &imageCount, nullptr));
	images.resize(imageCount);
	VKA(vkGetSwapchainImagesKHR(context->LogicalDevice(), swapchain, &imageCount, images.data()));
}

void VulkanSwapchain::CreateImageViews()
{
	imageViews.resize(images.size());
	for (uint32_t i = 0; i < images.size(); i++)
	{
		VkImageViewCreateInfo createInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		createInfo.image = images[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = context->surface->surfaceFormat.format;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		VKA(vkCreateImageView(context->LogicalDevice(), &createInfo, nullptr, &imageViews[i]))
	}
}