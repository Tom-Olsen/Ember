#ifndef __INCLUDE_GUARD_vmaBuffer_h__
#define __INCLUDE_GUARD_vmaBuffer_h__
#include <vulkan/vulkan.h>
#include "vulkanContext.h"
#include "vmaImage.h"



class VmaBuffer
{
public: // Members:
	VkBuffer buffer;
	VmaAllocation allocation;

private: // Members:
	VulkanContext* context;
	VkBufferCreateInfo bufferInfo;
	VmaAllocationCreateInfo allocInfo;

public: // Methods:
	VmaBuffer();
	VmaBuffer(VulkanContext* context, const VkBufferCreateInfo& bufferInfo, const VmaAllocationCreateInfo& allocInfo);
	~VmaBuffer();
	uint64_t GetSize();

public: // Static methods:
	static void CopyBufferToBuffer(VulkanContext* context, VmaBuffer* srcBuffer, VmaBuffer* dstBuffer, VkDeviceSize size, const VulkanQueue& queue);
	static void CopyBufferToImage(VulkanContext* context, VmaBuffer* srcBuffer, VmaImage* dstImage, const VulkanQueue& queue);
	static VmaBuffer StagingBuffer(VulkanContext* context, uint64_t size, void* inputData);
	static VmaBuffer StagingBuffer(VulkanContext* context, const std::vector<uint64_t>& sizes, const std::vector<void*>& inputDatas);
};



#endif // __INCLUDE_GUARD_vmaBuffer_h__