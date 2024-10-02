#ifndef __INCLUDE_GUARD_vulkanCommands_h__
#define __INCLUDE_GUARD_vulkanCommands_h__
#include <vulkan/vulkan.h>
#include <vector>
#include "vulkanLogicalDevice.h"



// TODO:
// -Change to VulkanCommand and replace VulkanCommands* with std::vector<VulkanCommand>.



/// <summary>
/// VulkanCommand class owns command pools and command buffers.
/// </summary>
class VulkanCommands
{
public: // Members:
	uint32_t size;	// should be equal to framesInFlight
	std::vector<VkCommandPool> pools;
	std::vector<VkCommandBuffer> buffers;

private: // Members:
	VulkanLogicalDevice* logicalDevice;

public: // Methods:
	VulkanCommands(uint32_t size, VulkanLogicalDevice* logicalDevice, VulkanQueue queue);
	~VulkanCommands();
};



#endif // __INCLUDE_GUARD_vulkanCommands_h__