#include <algorithm>
#include "vmaImage.h"
#include "vulkanHelper.h"
#include "macros.h"



// Constructors:
VmaImage::VmaImage(VulkanContext* context, const VkImageCreateInfo& imageInfo, const VmaAllocationCreateInfo& allocationInfo, const VkImageSubresourceRange& subresourceRange)
{
	this->context = context;
	this->imageInfo = imageInfo;
	this->allocationInfo = allocationInfo;
	this->subresourceRange = subresourceRange;

	// Create image:
	VKA(vmaCreateImage(context->Allocator(), &imageInfo, &allocationInfo, &image, &allocation, nullptr));

	// Create image view:
	VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	viewInfo.image = image;
	viewInfo.viewType = VkImageViewType((int)imageInfo.imageType);
	viewInfo.format = imageInfo.format;
	viewInfo.subresourceRange = subresourceRange;
	VKA(vkCreateImageView(context->LogicalDevice(), &viewInfo, nullptr, &imageView));
}



// Destructor:
VmaImage::~VmaImage()
{
	vmaDestroyImage(context->Allocator(), image, allocation);
	vkDestroyImageView(context->LogicalDevice(), imageView, nullptr);
}



// Getters:
uint64_t VmaImage::GetWidth()
{
	return imageInfo.extent.width;
}
uint64_t VmaImage::GetHeight()
{
	return imageInfo.extent.height;
}
uint64_t VmaImage::GetDepth()
{
	return imageInfo.extent.depth;
}



// Advanced methods:
void VmaImage::TransitionLayoutUndefinedToTransfer(VkImageSubresourceRange subresourceRange)
{
	// Transition is executed on transferQueue.
	VulkanCommand command = VulkanHelper::BeginSingleTimeCommand(context, context->logicalDevice->transferQueue);

	VkImageMemoryBarrier barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
	barrier.srcAccessMask = VK_ACCESS_NONE;					// types of memory access allowed before the barrier
	barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;	// types of memory access allowed after the barrier
	barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange = subresourceRange;

	VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;	// Immediatly
	VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;		// Before transfer stage
	vkCmdPipelineBarrier(
		command.buffer,
		srcStage, dstStage,
		0,	// dependency flags, typically 0
		0, nullptr,				// memory barriers
		0, nullptr,	// buffer memory barriers
		1, &barrier);	// image memory barriers

	VulkanHelper::EndSingleTimeCommand(context, command, context->logicalDevice->transferQueue);
}
void VmaImage::HandoffTransferToGraphicsQueue(VkImageSubresourceRange subresourceRange)
{
	// On transition ownership of the image is transferred from the transferQueue to the graphicsQueue.
	{
		VulkanCommand command = VulkanHelper::BeginSingleTimeCommand(context, context->logicalDevice->transferQueue);

		VkImageMemoryBarrier releaseBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		releaseBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		releaseBarrier.dstAccessMask = VK_ACCESS_NONE;
		releaseBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		releaseBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		releaseBarrier.srcQueueFamilyIndex = context->logicalDevice->transferQueue.familyIndex;
		releaseBarrier.dstQueueFamilyIndex = context->logicalDevice->graphicsQueue.familyIndex;
		releaseBarrier.image = image;
		releaseBarrier.subresourceRange = subresourceRange;

		VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;			// after transfer stage
		VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;	// wait for all transfers to finish
		vkCmdPipelineBarrier(
			command.buffer,
			srcStage, dstStage,
			0,	// dependency flags, typically 0
			0, nullptr,						// memory barriers
			0, nullptr,			// buffer memory barriers
			1, &releaseBarrier);	// image memory barriers

		VulkanHelper::EndSingleTimeCommand(context, command, context->logicalDevice->transferQueue);
	}
	{
		VulkanCommand command = VulkanHelper::BeginSingleTimeCommand(context, context->logicalDevice->graphicsQueue);

		VkImageMemoryBarrier acquireBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		acquireBarrier.srcAccessMask = VK_ACCESS_NONE;
		acquireBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;	// rdy for mipmapping
		acquireBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		acquireBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		acquireBarrier.srcQueueFamilyIndex = context->logicalDevice->transferQueue.familyIndex;
		acquireBarrier.dstQueueFamilyIndex = context->logicalDevice->graphicsQueue.familyIndex;
		acquireBarrier.image = image;
		acquireBarrier.subresourceRange = subresourceRange;

		VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;	// as early as possible
		VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;		// rdy for more transfer commands
		vkCmdPipelineBarrier(
			command.buffer,
			srcStage, dstStage,
			0,	// dependency flags, typically 0
			0, nullptr,						// memory barriers
			0, nullptr,			// buffer memory barriers
			1, &acquireBarrier);	// image memory barriers

		VulkanHelper::EndSingleTimeCommand(context, command, context->logicalDevice->graphicsQueue);
	}
}
void VmaImage::TransitionLayoutTransferToShaderRead(VkImageSubresourceRange subresourceRange)
{
	// On transition ownership of the image is transferred from the transferQueue to the graphicsQueue.
	{
		VulkanCommand command = VulkanHelper::BeginSingleTimeCommand(context, context->logicalDevice->transferQueue);

		VkImageMemoryBarrier releaseBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		releaseBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		releaseBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		releaseBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		releaseBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		releaseBarrier.srcQueueFamilyIndex = context->logicalDevice->transferQueue.familyIndex;
		releaseBarrier.dstQueueFamilyIndex = context->logicalDevice->graphicsQueue.familyIndex;
		releaseBarrier.image = image;
		releaseBarrier.subresourceRange = subresourceRange;

		VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;			// after transfer stage
		VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;	// before final stage
		vkCmdPipelineBarrier(
			command.buffer,
			srcStage, dstStage,
			0,	// dependency flags, typically 0
			0, nullptr,						// memory barriers
			0, nullptr,			// buffer memory barriers
			1, &releaseBarrier);	// image memory barriers

		VulkanHelper::EndSingleTimeCommand(context, command, context->logicalDevice->transferQueue);
	}
	{
		VulkanCommand command = VulkanHelper::BeginSingleTimeCommand(context, context->logicalDevice->graphicsQueue);

		VkImageMemoryBarrier acquireBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		acquireBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		acquireBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		acquireBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		acquireBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		acquireBarrier.srcQueueFamilyIndex = context->logicalDevice->transferQueue.familyIndex;
		acquireBarrier.dstQueueFamilyIndex = context->logicalDevice->graphicsQueue.familyIndex;
		acquireBarrier.image = image;
		acquireBarrier.subresourceRange = subresourceRange;

		VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;			// after transfer stage
		VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;	// before fragment shader stage
		vkCmdPipelineBarrier(
			command.buffer,
			srcStage, dstStage,
			0,	// dependency flags, typically 0
			0, nullptr,						// memory barriers
			0, nullptr,			// buffer memory barriers
			1, &acquireBarrier);	// image memory barriers

		VulkanHelper::EndSingleTimeCommand(context, command, context->logicalDevice->graphicsQueue);
	}
}
void VmaImage::GenerateMipmaps(uint32_t mipLevels)
{
	VulkanCommand command = VulkanHelper::BeginSingleTimeCommand(context, context->logicalDevice->graphicsQueue);

	VkImageMemoryBarrier barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
	barrier.image = image;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = 1;

	int mipWidth = static_cast<int>(GetWidth());
	int mipHeight = static_cast<int>(GetHeight());
	int mipDepth = static_cast<int>(GetDepth());
	for (uint32_t i = 1; i < mipLevels; i++)
	{
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(command.buffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
			0, nullptr,				// memory barriers
			0, nullptr,	// buffer memory barrier
			1, &barrier);	// image memory barriers

		VkImageBlit blit{};
		blit.srcOffsets[0] = { 0, 0, 0 };
		blit.srcOffsets[1] = { mipWidth, mipHeight, mipDepth };
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.dstOffsets[0] = { 0, 0, 0 };
		blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		vkCmdBlitImage(command.buffer,
			image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &blit,
			VK_FILTER_LINEAR);

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(command.buffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		mipWidth = std::max(1, mipWidth / 2);
		mipHeight = std::max(1, mipHeight / 2);
		mipDepth = std::max(1, mipDepth / 2);
	}

	barrier.subresourceRange.baseMipLevel = mipLevels - 1;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(command.buffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
		0, nullptr,
		0, nullptr,
		1, &barrier);

	VulkanHelper::EndSingleTimeCommand(context, command, context->logicalDevice->graphicsQueue);
}