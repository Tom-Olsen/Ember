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



/// <summary>
/// VulkanLogicalDevice class owns logical device and queues.
/// </summary>
class VulkanLogicalDevice
{
public: // Members:
	VkDevice device;
	VulkanQueue graphicsQueue;	// graphics and sync compute queue
	VulkanQueue presentQueue;	// present queue
	VulkanQueue computeQueue;	// async compute queue
	VulkanQueue transferQueue;	// async transfer queue

public: // Methods:
	VulkanLogicalDevice(VulkanPhysicalDevice* physicalDevice, VulkanSurface* surface, std::vector<const char*> deviceExtensions);
	~VulkanLogicalDevice();

private: // Methods:
	uint32_t FindGraphicsAndComputeQueueFamilyIndex(VkPhysicalDevice physicalDevice);
	uint32_t FindPresentQueueFamilyIndex(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
	uint32_t FindPureComputeQueueFamilyIndex(VkPhysicalDevice physicalDevice);
	uint32_t FindPureTransferQueueFamilyIndex(VkPhysicalDevice physicalDevice);
};



#endif // __INCLUDE_GUARD_vulkanLogicalDevice_h__