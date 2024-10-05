#include "vulkanDepthImage.h"
#include "vulkanHelper.h"
#include "macros.h"



// static members:
VkFormat VulkanDepthImage::format = VK_FORMAT_D32_SFLOAT_S8_UINT;



// Constructor:
VulkanDepthImage::VulkanDepthImage(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, VulkanSurface* surface)
{
	this->logicalDevice = logicalDevice;
	this->physicalDevice = physicalDevice;
	this->surface = surface;

	VkImageSubresourceRange subresourceRange;
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = 1;
	subresourceRange.baseArrayLayer = 0;
	subresourceRange.layerCount = 1;
	VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
	VkImageUsageFlags usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

	CreateImage(VK_IMAGE_TYPE_2D, format, tiling, usage, surface->CurrentExtent().width, surface->CurrentExtent().height, 1);
	AllocateAndBindMemory();
	CreateImageView(format, subresourceRange);
	TransitionLayout(subresourceRange);
}



// Destructor:
VulkanDepthImage::~VulkanDepthImage()
{
	vkDestroyImageView(logicalDevice->device, imageView, nullptr);
	vkDestroyImage(logicalDevice->device, image, nullptr);
	vkFreeMemory(logicalDevice->device, memory, nullptr);
}



// Private:
void VulkanDepthImage::CreateImage(VkImageType imageType, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, uint32_t width, uint32_t height, uint32_t depth)
{
	VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	imageInfo.imageType = imageType;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = depth;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format; // use the same format as texture pixels, otherwise the copy operation will fail
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = physicalDevice->maxMsaaSamples;
	imageInfo.flags = 0;

	VKA(vkCreateImage(logicalDevice->device, &imageInfo, nullptr, &image));
}
void VulkanDepthImage::AllocateAndBindMemory()
{
	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(logicalDevice->device, image, &memoryRequirements);

	VkMemoryAllocateInfo allocationInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
	allocationInfo.allocationSize = memoryRequirements.size;
	allocationInfo.memoryTypeIndex = VulkanHelper::FindMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VKA(vkAllocateMemory(logicalDevice->device, &allocationInfo, nullptr, &memory));
	vkBindImageMemory(logicalDevice->device, image, memory, 0);
}
void VulkanDepthImage::CreateImageView(VkFormat format, const VkImageSubresourceRange& subresourceRange)
{
	VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange = subresourceRange;
	VKA(vkCreateImageView(logicalDevice->device, &viewInfo, nullptr, &imageView));
}
void VulkanDepthImage::TransitionLayout(const VkImageSubresourceRange& subresourceRange)
{
	VulkanCommand command = VulkanHelper::BeginSingleTimeCommand(logicalDevice, logicalDevice->graphicsQueue);

	VkImageMemoryBarrier barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
	barrier.srcAccessMask = VK_ACCESS_NONE;					// types of memory access allowed before the barrier
	barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;	// types of memory access allowed after the barrier
	barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange = subresourceRange;

	VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;	// Immediatly
	VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;		// early fragment test stage
	vkCmdPipelineBarrier(
		command.buffer,
		srcStage, dstStage,
		0,	// dependency flags, typically 0
		0, nullptr,				// memory barriers
		0, nullptr,	// buffer memory barriers
		1, &barrier);	// image memory barriers

	VulkanHelper::EndSingleTimeCommand(logicalDevice, command, logicalDevice->graphicsQueue);
}