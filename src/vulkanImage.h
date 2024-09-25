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

protected: // Members:
	VulkanLogicalDevice* logicalDevice;
	VulkanPhysicalDevice* physicalDevice;

public: // Methods:
	VulkanImage(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice);
	~VulkanImage();
	void TransitionLayoutUndefinedToTransfer(VkImageSubresourceRange subresourceRange);
	void TransitionLayoutTransferToShaderRead(VkImageSubresourceRange subresourceRange);

protected: // Methods:
	void CreateImage(VkImageType imageType, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, uint32_t width, uint32_t height, uint32_t depth);
	void AllocateAndBindMemory();
};



class VulkanImage1d : public VulkanImage
{
public: // Methods:
	VulkanImage1d(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, int width);
};



class VulkanImage2d : public VulkanImage
{
public: // Methods:
	VulkanImage2d(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, int width, int height);
};



class VulkanImage3d : public VulkanImage
{
public: // Methods:
	VulkanImage3d(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, int width, int height, int depth);
};



#endif // __INCLUDE_GUARD_vulkanImage_h__