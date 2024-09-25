#ifndef __INCLUDE_GUARD_vulkanBuffer_h__
#define __INCLUDE_GUARD_vulkanBuffer_h__
#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
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
	VulkanBuffer(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, uint64_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryPropertyFlags, const std::vector<uint32_t>& queueFamilyIndices = {});
	~VulkanBuffer();
	static void CopyBufferToBuffer(VulkanLogicalDevice* logicalDevice, VulkanBuffer* srcBuffer, VulkanBuffer* dstBuffer, VkDeviceSize size, const VulkanQueue& queue);
	static VulkanBuffer StagingBuffer(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, uint64_t bufferSize, void* inputData);
	static VulkanBuffer StagingBuffer(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, const std::vector<uint64_t>& bufferSizes, const std::vector<void*>& inputDatas);
};



#endif // __INCLUDE_GUARD_vulkanBuffer_h__