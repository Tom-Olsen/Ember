#ifndef __INCLUDE_GUARD_vulkanBuffer_h__
#define __INCLUDE_GUARD_vulkanBuffer_h__
#include <vulkan/vulkan.h>
#include "vulkanLogicalDevice.h"
#include "vulkanPhysicalDevice.h"



/// <summary>
/// VulkanBuffer owns memory block (gets deletet in destructor).
/// Note: Only 4096 memory blocks can be allocated.
/// Multiple buffers on one memory block possible, but not implemented here.
/// </summary>
class VulkanBuffer
{
public: // Members:
	VulkanLogicalDevice* logicalDevice;
	VulkanPhysicalDevice* physicalDevice;
	VkBuffer buffer;
	VkDeviceMemory memory;

public: // Methods:
	/// <param name="logicalDevice"></param>
	/// <param name="physicalDevice"></param>
	/// <param name="size">in bytes</param>
	/// <param name="usage"></param>
	/// <param name="memoryPropertyFlags"></param>
	VulkanBuffer(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, uint64_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryPropertyFlags);
	~VulkanBuffer();

private: // Methods:
	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};



#endif // __INCLUDE_GUARD_vulkanBuffer_h__