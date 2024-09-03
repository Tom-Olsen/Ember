#ifndef __INCLUDE_GUARD_vulkanCommands_h__
#define __INCLUDE_GUARD_vulkanCommands_h__
#include <vulkan/vulkan.h>
#include <vector>
#include "vulkanLogicalDevice.h"



class VulkanCommands
{
private: // Members:
	VulkanLogicalDevice* logicalDevice;


public: // Members:
	size_t size;	// should be equal to framesInFlight
	std::vector<VkCommandPool> pools;
	std::vector<VkCommandBuffer> buffers;

public: // Methods:
	VulkanCommands(size_t size, VulkanLogicalDevice* logicalDevice);
	~VulkanCommands();
};



#endif // __INCLUDE_GUARD_vulkanCommands_h__