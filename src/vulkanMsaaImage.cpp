#include "vulkanMsaaImage.h"
#include "vulkanHelper.h"
#include "macros.h"



// Constructor:
VulkanMsaaImage::VulkanMsaaImage(VulkanContext* context)
{
	this->context = context;

	VkImageSubresourceRange subresourceRange;
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = 1;
	subresourceRange.baseArrayLayer = 0;
	subresourceRange.layerCount = 1;

	VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = context->surface->CurrentExtent().width;
	imageInfo.extent.height = context->surface->CurrentExtent().height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = context->surface->surfaceFormat.format;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = context->physicalDevice->maxMsaaSamples;
	imageInfo.flags = 0;

	VmaAllocationCreateInfo allocationInfo = {};
	allocationInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
	allocationInfo.flags = 0;
	allocationInfo.requiredFlags = 0;
	allocationInfo.preferredFlags = 0;

	this->image = std::make_unique<VmaImage>(context, imageInfo, allocationInfo, subresourceRange);
}

// Destructor:
VulkanMsaaImage::~VulkanMsaaImage()
{

}