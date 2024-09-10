#ifndef __INCLUDE_GUARD_vulkanBuffer_h__
#define __INCLUDE_GUARD_vulkanBuffer_h__
#include <vulkan/vulkan.h>
#include "vulkanLogicalDevice.h"
#include "vulkanPhysicalDevice.h"



/// <summary>
/// VulkanBuffer class owns memory block (gets deletet in destructor).
/// Note: Only 4096 memory blocks can be allocated.
/// TODO: Multiple buffers on one memory block possible, but not implemented yet.
/// </summary>
class VulkanBuffer
{
public: // Members:
	uint64_t size;
	VkBuffer buffer;
	VkDeviceMemory memory;

private: // Members:
	VulkanLogicalDevice* logicalDevice;
	VulkanPhysicalDevice* physicalDevice;

public: // Methods:
	VulkanBuffer(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, uint64_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryPropertyFlags, std::vector<uint32_t> queueFamilyIndices = {});
	~VulkanBuffer();
	static void CopyBuffer(VulkanLogicalDevice* logicalDevice, VulkanBuffer* srcBuffer, VulkanBuffer* dstBuffer, VkDeviceSize size);

private: // Methods:
	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};



#endif // __INCLUDE_GUARD_vulkanBuffer_h__