#ifndef __INCLUDE_GUARD_vmaBuffer_h__
#define __INCLUDE_GUARD_vmaBuffer_h__
#include "vk_mem_alloc.h"
#include <memory>
#include <vulkan/vulkan.h>
#include <vector>



namespace emberEngine
{
	// Forward declarations:
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
		VmaBuffer(VmaBuffer&& other) noexcept;				// move constructor
		VmaBuffer& operator=(VmaBuffer&& other) noexcept;	// move assignment
		~VmaBuffer();

		// No copying of VmaBuffers allowed:
		VmaBuffer(const VmaBuffer&) = delete;
		VmaBuffer& operator=(const VmaBuffer&) = delete;

		// Getters:
		const VkBuffer& GetVkBuffer() const;
		const VmaAllocation& GetVmaAllocation() const;
		const VkBufferCreateInfo& GetVkBufferCreateInfo() const;
		const VmaAllocationCreateInfo& GetVmaAllocationCreateInfo() const;
		uint64_t GetSize();

		// Static methods:
		static void CopyBufferToBuffer(VulkanContext* pContext, VmaBuffer* srcBuffer, VmaBuffer* dstBuffer, VkDeviceSize size, const VulkanQueue& queue);
		static void CopyBufferToImage(VulkanContext* pContext, VmaBuffer* srcBuffer, VmaImage* dstImage, const VulkanQueue& queue, uint32_t layerCount);
		static VmaBuffer StagingBuffer(VulkanContext* pContext, uint64_t size, void* inputData);
		static VmaBuffer StagingBuffer(VulkanContext* pContext, const std::vector<uint64_t>& sizes, const std::vector<void*>& inputDatas);
	};
}



#endif // __INCLUDE_GUARD_vmaBuffer_h__