#ifndef __INCLUDE_GUARD_vulkanPhysicalDevice_h__
#define __INCLUDE_GUARD_vulkanPhysicalDevice_h__
#include <vulkan/vulkan.h>
#include <vector>
#include "vulkanInstance.h"



class VulkanPhysicalDevice
{
public: // Members:
	VkPhysicalDevice device;

public: // Methods:
	VulkanPhysicalDevice(VulkanInstance* instance);

private: // Methods:
	int DeviceScore(VkPhysicalDevice device);
	VkBool32 HasGraphicsAndComputeQueueFamily(VkPhysicalDevice device);
	VkBool32 HasPresentQueueFamily(VkPhysicalDevice device, VkSurfaceKHR surface);
};



#endif // __INCLUDE_GUARD_vulkanPhysicalDevice_h__