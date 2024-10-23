#include "vulkanDepthImage.h"
#include "vulkanHelper.h"
#include "macros.h"



// static members:
VkFormat VulkanDepthImage::depthFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;



// Constructor:
VulkanDepthImage::VulkanDepthImage(VulkanContext* context)
{
	this->context = context;

	VkImageSubresourceRange subresourceRange;
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
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
	imageInfo.format = depthFormat;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = context->physicalDevice->maxMsaaSamples;
	imageInfo.flags = 0;

	VmaAllocationCreateInfo allocationInfo = {};
	allocationInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
	allocationInfo.flags = 0;
	allocationInfo.requiredFlags = 0;
	allocationInfo.preferredFlags = 0;

	this->image = std::make_unique<VmaImage>(context, imageInfo, allocationInfo, subresourceRange);
	TransitionLayout(subresourceRange);
}



// Destructor:
VulkanDepthImage::~VulkanDepthImage()
{

}



void VulkanDepthImage::TransitionLayout(const VkImageSubresourceRange& subresourceRange)
{
	VulkanCommand command = VulkanHelper::BeginSingleTimeCommand(context, context->logicalDevice->graphicsQueue);

	VkImageMemoryBarrier barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
	barrier.srcAccessMask = VK_ACCESS_NONE;					// types of memory access allowed before the barrier
	barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;	// types of memory access allowed after the barrier
	barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image->image;
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

	VulkanHelper::EndSingleTimeCommand(context, command, context->logicalDevice->graphicsQueue);
}