#include <algorithm>
#include "vulkanImage.h"
#include "vulkanHelper.h"
#include "macros.h"



// Constructors:
VulkanImage::VulkanImage(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice)
	: logicalDevice(logicalDevice), physicalDevice(physicalDevice) {}
VulkanImage1d::VulkanImage1d(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, int width)
	: VulkanImage(logicalDevice, physicalDevice)
{
	this->width = std::max(width, 1);
	this->height = 1;
	this->depth = 1;
	CreateImage(VK_IMAGE_TYPE_1D, format, tiling, usage, this->width, 1, 1);
	AllocateAndBindMemory();
}
VulkanImage2d::VulkanImage2d(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, int width, int height)
	: VulkanImage(logicalDevice, physicalDevice)
{
	this->width = std::max(width, 1);
	this->height = std::max(height, 1);
	this->depth = 1;
	CreateImage(VK_IMAGE_TYPE_2D, format, tiling, usage, this->width, this->height, 1);
	AllocateAndBindMemory();
}
VulkanImage3d::VulkanImage3d(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, int width, int height, int depth)
	: VulkanImage(logicalDevice, physicalDevice)
{
	this->width = std::max(width, 1);
	this->height = std::max(height, 1);
	this->depth = std::max(depth, 1);
	CreateImage(VK_IMAGE_TYPE_3D, format, tiling, usage, this->width, this->height, this->depth);
	AllocateAndBindMemory();
}



// Destructor:
VulkanImage::~VulkanImage()
{
	vkDestroyImage(logicalDevice->device, image, nullptr);
	vkFreeMemory(logicalDevice->device, memory, nullptr);
}



// Public:
void VulkanImage::TransitionLayoutUndefinedToTransfer(VkImageSubresourceRange subresourceRange)
{
	// Transition is executed on transferQueue.
	VulkanCommands commands = VulkanHelper::BeginSingleTimeCommands(logicalDevice, logicalDevice->transferQueue);

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
		commands.buffers[0],
		srcStage, dstStage,
		0,	// dependency flags, typically 0
		0, nullptr,				// memory barriers
		0, nullptr,	// buffer memory barriers
		1, &barrier);	// image memory barriers

	VulkanHelper::EndSingleTimeCommands(logicalDevice, commands, logicalDevice->transferQueue);
}
void VulkanImage::TransitionLayoutTransferToShaderRead(VkImageSubresourceRange subresourceRange)
{
	// On transition ownership of the image is transferred from the transferQueue to the graphicsQueue.
	{
		VulkanCommands commands = VulkanHelper::BeginSingleTimeCommands(logicalDevice, logicalDevice->transferQueue);

		VkImageMemoryBarrier releaseBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		releaseBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		releaseBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		releaseBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		releaseBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		releaseBarrier.srcQueueFamilyIndex = logicalDevice->transferQueue.familyIndex;
		releaseBarrier.dstQueueFamilyIndex = logicalDevice->graphicsQueue.familyIndex;
		releaseBarrier.image = image;
		releaseBarrier.subresourceRange = subresourceRange;

		VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;			// after transfer stage
		VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;	// before final stage
		vkCmdPipelineBarrier(
			commands.buffers[0],
			srcStage, dstStage,
			0,	// dependency flags, typically 0
			0, nullptr,						// memory barriers
			0, nullptr,			// buffer memory barriers
			1, &releaseBarrier);	// image memory barriers

		VulkanHelper::EndSingleTimeCommands(logicalDevice, commands, logicalDevice->transferQueue);
	}
	{
		VulkanCommands commands = VulkanHelper::BeginSingleTimeCommands(logicalDevice, logicalDevice->graphicsQueue);

		VkImageMemoryBarrier acquireBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		acquireBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		acquireBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		acquireBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		acquireBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		acquireBarrier.srcQueueFamilyIndex = logicalDevice->transferQueue.familyIndex;
		acquireBarrier.dstQueueFamilyIndex = logicalDevice->graphicsQueue.familyIndex;
		acquireBarrier.image = image;
		acquireBarrier.subresourceRange = subresourceRange;

		VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;			// after transfer stage
		VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;	// before fragment shader stage
		vkCmdPipelineBarrier(
			commands.buffers[0],
			srcStage, dstStage,
			0,	// dependency flags, typically 0
			0, nullptr,						// memory barriers
			0, nullptr,			// buffer memory barriers
			1, &acquireBarrier);	// image memory barriers

		VulkanHelper::EndSingleTimeCommands(logicalDevice, commands, logicalDevice->graphicsQueue);
	}
}



// Protected:
void VulkanImage::CreateImage(VkImageType imageType, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, uint32_t width, uint32_t height, uint32_t depth)
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
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = 0;

	VKA(vkCreateImage(logicalDevice->device, &imageInfo, nullptr, &image));
}
void VulkanImage::AllocateAndBindMemory()
{
	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(logicalDevice->device, image, &memoryRequirements);

	VkMemoryAllocateInfo allocationInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
	allocationInfo.allocationSize = memoryRequirements.size;
	allocationInfo.memoryTypeIndex = VulkanHelper::FindMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VKA(vkAllocateMemory(logicalDevice->device, &allocationInfo, nullptr, &memory));
	vkBindImageMemory(logicalDevice->device, image, memory, 0);
}