#ifndef __INCLUDE_GUARD_vulkanRendererBackend_vmaBuffer_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_vmaBuffer_h__
#include "vk_mem_alloc.h"
#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct DeviceQueue;
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
		~VmaBuffer();

		// Non-copyable:
		VmaBuffer(const VmaBuffer&) = delete;
		VmaBuffer& operator=(const VmaBuffer&) = delete;

		// Movable:
		VmaBuffer(VmaBuffer&& other) noexcept;
		VmaBuffer& operator=(VmaBuffer&& other) noexcept;

		// Getters:
		const std::string& GetName() const;
		const VkBuffer& GetVkBuffer() const;
		const VmaAllocation& GetVmaAllocation() const;
		const VkBufferCreateInfo& GetVkBufferCreateInfo() const;
		const VmaAllocationCreateInfo& GetVmaAllocationCreateInfo() const;
		uint64_t GetSize();

	private: // Methods:
		void Cleanup();
		void MoveFrom(VmaBuffer& other) noexcept;
	};
}



#endif // __INCLUDE_GUARD_vulkanRendererBackend_vmaBuffer_h__