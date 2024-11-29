#ifndef __INCLUDE_GUARD_vulkanPhysicalDevice_h__
#define __INCLUDE_GUARD_vulkanPhysicalDevice_h__
#include <vulkan/vulkan.h>
#include <vector>
#include "vulkanInstance.h"



/// <summary>
/// VulkanPhysicalDevice wrapper.
/// </summary>
class VulkanPhysicalDevice
{
public: // Members:
	VkPhysicalDevice device;
	VkSampleCountFlagBits maxMsaaSamples;
	VkBool32 supportsDepthClamp = false;
	VkBool32 supportsDepthBias = false;
	VkBool32 supportsMultiViewport = false;


public: // Methods:
	VulkanPhysicalDevice(VulkanInstance* instance);
	~VulkanPhysicalDevice();

private: // Methods:
	int DeviceScore(VkPhysicalDevice device);
	VkBool32 HasGraphicsAndComputeQueueFamily(VkPhysicalDevice device);
	VkBool32 HasPresentQueueFamily(VkPhysicalDevice device, VkSurfaceKHR surface);
	VkSampleCountFlagBits MaxUsableMsaaSampleCount();
};



#endif // __INCLUDE_GUARD_vulkanPhysicalDevice_h__