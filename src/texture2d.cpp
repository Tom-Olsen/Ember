// needs to be defined before including stb_image.h, but may not be in the header file!
#define STB_IMAGE_IMPLEMENTATION 
#include "texture2d.h"
#include <iostream>
#include "vulkanHelper.h"
#include "macros.h"



// Constructor:
Texture2d::Texture2d(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, VulkanSampler* sampler, char const* filename)
{
	this->logicalDevice = logicalDevice;
	this->physicalDevice = physicalDevice;
	this->sampler = sampler;

	// Load image:
	// STBI_rgb_alpha = 4 8-bit channels
	pixels = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);
	if (!pixels)
		throw std::runtime_error("failed to load texture image!");
	this->depth = 1;

	// Create staging buffer:
	uint64_t bufferSize = 4 * width * height;
	VulkanBuffer stagingBuffer = VulkanBuffer::StagingBuffer(logicalDevice, physicalDevice, bufferSize, pixels);
	
	// Free image:
	stbi_image_free(pixels);

	// Create image:
	VkImageSubresourceRange subresourceRange;
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = 1;
	subresourceRange.baseArrayLayer = 0;
	subresourceRange.layerCount = 1;
	VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
	VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
	VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	image = std::make_shared<VulkanImage2d>(logicalDevice, physicalDevice, format, tiling, usage, width, height);

	// Transition image layout:
	image->TransitionLayoutUndefinedToTransfer(subresourceRange);
	VulkanHelper::CopyBufferToImage(logicalDevice, &stagingBuffer, image.get(), logicalDevice->transferQueue);
	image->TransitionLayoutTransferToShaderRead(subresourceRange);

	// Create image view:
	VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	viewInfo.image = image->image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange = subresourceRange;
	VKA(vkCreateImageView(logicalDevice->device, &viewInfo, nullptr, &imageView));
}



// Destructor:
Texture2d::~Texture2d()
{
	vkDestroyImageView(logicalDevice->device, imageView, nullptr);
}