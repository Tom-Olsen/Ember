#ifndef __INCLUDE_GUARD_vmaBuffer_h__
#define __INCLUDE_GUARD_vmaBuffer_h__
#include "vk_mem_alloc.h"
#include <vulkan/vulkan.h>
#include <vector>



struct VulkanContext;
struct VulkanQueue;
class VmaImage;



class VmaBuffer
{
private: // Members:
	VkBuffer m_buffer;
	VmaAllocation m_allocation;
	VkBufferCreateInfo m_bufferInfo;
	VmaAllocationCreateInfo m_allocInfo;
	VulkanContext* m_pContext;

public: // Methods:
	VmaBuffer();
	VmaBuffer(VulkanContext* pContext, const VkBufferCreateInfo& bufferInfo, const VmaAllocationCreateInfo& allocInfo);
	~VmaBuffer();

	// Getters:
	const VkBuffer& GetVkBuffer() const;
	const VmaAllocation& GetVmaAllocation() const;
	const VkBufferCreateInfo& GetVkBufferCreateInfo() const;
	const VmaAllocationCreateInfo& GetVmaAllocationCreateInfo() const;
	uint64_t GetSize();

	// Static methods:
	static void CopyBufferToBuffer(VulkanContext* context, VmaBuffer* srcBuffer, VmaBuffer* dstBuffer, VkDeviceSize size, const VulkanQueue& queue);
	static void CopyBufferToImage(VulkanContext* context, VmaBuffer* srcBuffer, VmaImage* dstImage, const VulkanQueue& queue, uint32_t layerCount);
	static VmaBuffer StagingBuffer(VulkanContext* context, uint64_t size, void* inputData);
	static VmaBuffer StagingBuffer(VulkanContext* context, const std::vector<uint64_t>& sizes, const std::vector<void*>& inputDatas);
};



#endif // __INCLUDE_GUARD_vmaBuffer_h__