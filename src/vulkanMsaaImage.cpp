#include "vulkanMsaaImage.h"
#include "vulkanHelper.h"
#include "macros.h"



// Constructor:
VulkanMsaaImage::VulkanMsaaImage(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, VulkanSurface* surface)
{
	this->logicalDevice = logicalDevice;
	this->physicalDevice = physicalDevice;
	this->width = surface->CurrentExtent().width;
	this->height = surface->CurrentExtent().height;

	// Create Image:
	VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = surface->surfaceFormat.format;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = physicalDevice->maxMsaaSamples;
	imageInfo.flags = 0;
	VKA(vkCreateImage(logicalDevice->device, &imageInfo, nullptr, &image));

	// Allocate and Bind Memory:
	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(logicalDevice->device, image, &memoryRequirements);

	VkMemoryAllocateInfo allocationInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
	allocationInfo.allocationSize = memoryRequirements.size;
	allocationInfo.memoryTypeIndex = VulkanHelper::FindMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VKA(vkAllocateMemory(logicalDevice->device, &allocationInfo, nullptr, &memory));
	vkBindImageMemory(logicalDevice->device, image, memory, 0);

	// Create Image View:
	VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = surface->surfaceFormat.format;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;
	VKA(vkCreateImageView(logicalDevice->device, &viewInfo, nullptr, &imageView));
}



// Destructor:
VulkanMsaaImage::~VulkanMsaaImage()
{
	vkDestroyImageView(logicalDevice->device, imageView, nullptr);
	vkDestroyImage(logicalDevice->device, image, nullptr);
	vkFreeMemory(logicalDevice->device, memory, nullptr);
}