#ifndef __INCLUDE_GUARD_vmaBuffer_h__
#define __INCLUDE_GUARD_vmaBuffer_h__
#include "vk_mem_alloc.h"
#include <memory>
#include <string>
#include <vulkan/vulkan.h>
#include <vector>



namespace emberEngine
{
	// Forward declarations:
	struct VulkanQueue;
	class VmaImage;



	class VmaBuffer
	{
	private: // Members:
		static uint32_t s_index;
		std::string m_name;
		VkBuffer m_buffer;
		VmaAllocation m_allocation;
		VkBufferCreateInfo m_bufferInfo;
		VmaAllocationCreateInfo m_allocInfo;

	public: // Methods:
		VmaBuffer(const std::string& name, const VkBufferCreateInfo& bufferInfo, const VmaAllocationCreateInfo& allocInfo);
		VmaBuffer(VmaBuffer&& other) noexcept;				// move constructor
		VmaBuffer& operator=(VmaBuffer&& other) noexcept;	// move assignment
		~VmaBuffer();

		// No copying of VmaBuffers allowed:
		VmaBuffer(const VmaBuffer&) = delete;
		VmaBuffer& operator=(const VmaBuffer&) = delete;

		// Getters:
		const std::string& GetName() const;
		const VkBuffer& GetVkBuffer() const;
		const VmaAllocation& GetVmaAllocation() const;
		const VkBufferCreateInfo& GetVkBufferCreateInfo() const;
		const VmaAllocationCreateInfo& GetVmaAllocationCreateInfo() const;
		uint64_t GetSize();

		// Static methods:
		static void CopyBufferToBuffer(VmaBuffer* srcBuffer, VmaBuffer* dstBuffer, VkDeviceSize size, const VulkanQueue& queue);
		static void CopyBufferToImage(VmaBuffer* srcBuffer, VmaImage* dstImage, const VulkanQueue& queue, uint32_t layerCount);
		static VmaBuffer StagingBuffer(uint64_t size, void* inputData);
		static VmaBuffer StagingBuffer(const std::vector<uint64_t>& sizes, const std::vector<void*>& inputDatas);
	};
}



#endif // __INCLUDE_GUARD_vmaBuffer_h__