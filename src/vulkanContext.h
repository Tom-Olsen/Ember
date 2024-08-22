#ifndef __INCLUDE_GUARD_vulkanContext_h__
#define __INCLUDE_GUARD_vulkanContext_h__
#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
#include "vulkanInstance.h"
#include "vulkanPhysicalDevice.h"
#include "vulkanLogicalDevice.h"



class VulkanContext
{
public: // Members:
	std::unique_ptr<VulkanInstance> instance;
	std::unique_ptr<VulkanPhysicalDevice> physicalDevice;
	std::unique_ptr<VulkanLogicalDevice> logicalDevice;

public: // Methods:
	VulkanContext(std::vector<const char*> instanceExtensions, std::vector<const char*> deviceExtensions);
	~VulkanContext();
};
#endif // __INCLUDE_GUARD_vulkanContext_h__