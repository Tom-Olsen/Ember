#ifndef __INCLUDE_GUARD_vulkanCommands_h__
#define __INCLUDE_GUARD_vulkanCommands_h__
#include <vulkan/vulkan.h>
#include <vector>
#include "vulkanLogicalDevice.h"



/// <summary>
/// VulkanCommand class owns command pools and command buffers.
/// </summary>
class VulkanCommands
{
public: // Members:
	size_t size;	// should be equal to framesInFlight
	std::vector<VkCommandPool> pools;
	std::vector<VkCommandBuffer> buffers;

private: // Members:
	VulkanLogicalDevice* logicalDevice;

public: // Methods:
	VulkanCommands(size_t size, VulkanLogicalDevice* logicalDevice, VulkanQueue queue);
	~VulkanCommands();
};



#endif // __INCLUDE_GUARD_vulkanCommands_h__