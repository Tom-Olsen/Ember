#pragma once
#ifndef __INCLUDE_GUARD_texture_h__
#define __INCLUDE_GUARD_texture_h__
#include <vulkan/vulkan.h>
#include <memory>
#include "vulkanLogicalDevice.h"
#include "vulkanPhysicalDevice.h"
#include "vulkanBuffer.h"
#include "vulkanImage.h"
#include "vulkanSampler.h"
#include "stb_image.h"



class Texture2d
{
public: // Members:
	int width;
	int height;
	int depth;
	int channels;
	std::shared_ptr<VulkanImage2d> image;
	VkImageView imageView;
	VulkanSampler* sampler;

private: // Members:
	VulkanLogicalDevice* logicalDevice;
	VulkanPhysicalDevice* physicalDevice;
	stbi_uc* pixels;

public: // Methods:
	Texture2d(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, VulkanSampler* sampler, char const* filename);
	~Texture2d();
};



#endif // __INCLUDE_GUARD_texture_h__