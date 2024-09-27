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
	this->depth = 1;

	// Load image:
	// STBI_rgb_alpha = 4 8-bit channels
	pixels = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);
	if (!pixels)
		throw std::runtime_error("failed to load texture image!");
	mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

	// Create staging buffer:
	uint64_t bufferSize = 4 * width * height;
	VulkanBuffer stagingBuffer = VulkanBuffer::StagingBuffer(logicalDevice, physicalDevice, bufferSize, pixels);
	
	// Free image:
	stbi_image_free(pixels);

	// Create image:
	VkImageSubresourceRange subresourceRange;
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = mipLevels;
	subresourceRange.baseArrayLayer = 0;
	subresourceRange.layerCount = 1;
	VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
	VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
	VkImageUsageFlags usage =  VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	image = std::make_shared<VulkanImage2d>(logicalDevice, physicalDevice, format, tiling, usage, subresourceRange, width, height);

	// Transition image layout and create mipLevels:
	image->TransitionLayoutUndefinedToTransfer(subresourceRange);
	VulkanHelper::CopyBufferToImage(logicalDevice, &stagingBuffer, image.get(), logicalDevice->transferQueue);
	image->HandoffTransferToGraphicsQueue(subresourceRange);
	image->GenerateMipmaps(mipLevels);
	// old version that does final image transition and handoff between transfer and graphics queue, but no mipmapping:
	//image->TransitionLayoutTransferToShaderRead(subresourceRange);
}



// Destructor:
Texture2d::~Texture2d()
{

}