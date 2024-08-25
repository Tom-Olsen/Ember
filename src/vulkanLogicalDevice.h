#ifndef __INCLUDE_GUARD_vulkanLogicalDevice_h__
#define __INCLUDE_GUARD_vulkanLogicalDevice_h__
#include <vulkan/vulkan.h>
#include <vector>
#include <set>
#include "vulkanInstance.h"
#include "vulkanPhysicalDevice.h"
#include "vulkanSurface.h"



struct VulkanQueue
{
	VkQueue queue;
	uint32_t familyIndex;
};



class VulkanLogicalDevice
{
public: // Members:
	VkDevice device;
	VulkanQueue graphicsQueue;
	VulkanQueue presentQueue;
	VulkanQueue computeQueue;

public: // Methods:
	VulkanLogicalDevice(VulkanPhysicalDevice* physicalDevice, VulkanSurface* surface, std::vector<const char*> deviceExtensions);
	~VulkanLogicalDevice();

private: // Methods:
	uint32_t FindGraphicsAndComputeQueueFamilyIndex(VkPhysicalDevice physicalDevice);
	uint32_t FindPresentQueueFamilyIndex(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
	uint32_t FindPureComputeQueueFamilyIndex(VkPhysicalDevice physicalDevice);
};



#endif // __INCLUDE_GUARD_vulkanLogicalDevice_h__