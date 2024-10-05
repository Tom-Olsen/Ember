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
class VulkanCommand
{
public: // Members:
	VkCommandPool pool;
	VkCommandBuffer buffer;

private: // Members:
	VulkanLogicalDevice* logicalDevice;

public: // Methods:
	VulkanCommand(VulkanLogicalDevice* logicalDevice, VulkanQueue queue);
	~VulkanCommand();
};



#endif // __INCLUDE_GUARD_vulkanCommands_h__