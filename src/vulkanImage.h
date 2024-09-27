#pragma once
#ifndef __INCLUDE_GUARD_vulkanImage_h__
#define __INCLUDE_GUARD_vulkanImage_h__
#include <vulkan/vulkan.h>
#include "vulkanLogicalDevice.h"
#include "vulkanPhysicalDevice.h"



class VulkanImage
{
public: // Members:
	int width;
	int height;
	int depth;
	VkImage image;
	VkDeviceMemory memory;
	VkImageView imageView;

protected: // Members:
	VulkanLogicalDevice* logicalDevice;
	VulkanPhysicalDevice* physicalDevice;

public: // Methods:
	VulkanImage(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice);
	~VulkanImage();
	void TransitionLayoutUndefinedToTransfer(VkImageSubresourceRange subresourceRange);
	void HandoffTransferToGraphicsQueue(VkImageSubresourceRange subresourceRange);
	void TransitionLayoutTransferToShaderRead(VkImageSubresourceRange subresourceRange);
	void GenerateMipmaps(uint32_t mipLevels);

protected: // Methods:
	void CreateImage(VkImageType imageType, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, uint32_t mipLevels, uint32_t width, uint32_t height, uint32_t depth);
	void AllocateAndBindMemory();
	void CreateImageView(VkImageViewType imageType, VkFormat format, VkImageSubresourceRange subresourceRange);
};



class VulkanImage1d : public VulkanImage
{
public: // Methods:
	VulkanImage1d(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImageSubresourceRange subresourceRange, int width);
};



class VulkanImage2d : public VulkanImage
{
public: // Methods:
	VulkanImage2d(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImageSubresourceRange subresourceRange, int width, int height);
};



class VulkanImage3d : public VulkanImage
{
public: // Methods:
	VulkanImage3d(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImageSubresourceRange subresourceRange, int width, int height, int depth);
};



#endif // __INCLUDE_GUARD_vulkanImage_h__