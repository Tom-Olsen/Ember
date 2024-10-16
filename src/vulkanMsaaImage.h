#pragma once
#ifndef __INCLUDE_GUARD_vulkanMsaaImage_h__
#define __INCLUDE_GUARD_vulkanMsaaImage_h__
#include <vulkan/vulkan.h>
#include "vulkanContext.h"
#include "vmaImage.h"



class VulkanMsaaImage
{
public: // Members:
	std::unique_ptr<VmaImage> image;

private: // Members:
	VulkanContext* context;

public: // Methods:
	VulkanMsaaImage(VulkanContext* context);
	~VulkanMsaaImage();
};



#endif // __INCLUDE_GUARD_vulkanMsaaImage_h__