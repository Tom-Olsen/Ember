#ifndef __INCLUDE_GUARD_vmaBuffer_h__
#define __INCLUDE_GUARD_vmaBuffer_h__
#include "vk_mem_alloc.h"
#include <memory>
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
	std::unique_ptr<VkBufferCreateInfo> m_pBufferInfo;
	std::unique_ptr<VmaAllocationCreateInfo> m_pAllocInfo;
	VulkanContext* m_pContext;

public: // Methods:
	VmaBuffer();
	VmaBuffer(VulkanContext* pContext, VkBufferCreateInfo* pBufferInfo, VmaAllocationCreateInfo* pAllocInfo);
	VmaBuffer(VmaBuffer&& other) noexcept;				// move constructor
	VmaBuffer& operator=(VmaBuffer&& other) noexcept;	// move assignment
	~VmaBuffer();

	// No copying of VmaBuffers allowed:
	VmaBuffer(const VmaBuffer&) = delete;
	VmaBuffer& operator=(const VmaBuffer&) = delete;

	// Getters:
	const VkBuffer& GetVkBuffer() const;
	const VmaAllocation& GetVmaAllocation() const;
	const VkBufferCreateInfo* const GetVkBufferCreateInfo() const;
	const VmaAllocationCreateInfo* const GetVmaAllocationCreateInfo() const;
	uint64_t GetSize();

	// Static methods:
	static void CopyBufferToBuffer(VulkanContext* context, VmaBuffer* srcBuffer, VmaBuffer* dstBuffer, VkDeviceSize size, const VulkanQueue& queue);
	static void CopyBufferToImage(VulkanContext* context, VmaBuffer* srcBuffer, VmaImage* dstImage, const VulkanQueue& queue, uint32_t layerCount);
	static VmaBuffer StagingBuffer(VulkanContext* context, uint64_t size, void* inputData);
	static VmaBuffer StagingBuffer(VulkanContext* context, const std::vector<uint64_t>& sizes, const std::vector<void*>& inputDatas);
};



#endif // __INCLUDE_GUARD_vmaBuffer_h__