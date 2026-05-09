#pragma once
#include "vk_mem_alloc.h"
#include "vulkanBufferCreateInfo.h"
#include <string>



// Forward declarations:
typedef struct VkBuffer_T* VkBuffer;



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct DeviceQueue;
	class VmaImage;



	class VmaBuffer
	{
	private: // Members:
		static uint32_t s_index;
		VkBuffer m_buffer;
		VmaAllocation m_allocation;
		BufferCreateInfo m_bufferInfo;
		VmaAllocationCreateInfo m_allocationInfo;

	public: // Methods:
		VmaBuffer(const BufferCreateInfo& bufferInfo, const VmaAllocationCreateInfo& allocInfo);
		~VmaBuffer();

		// Non-copyable:
		VmaBuffer(const VmaBuffer&) = delete;
		VmaBuffer& operator=(const VmaBuffer&) = delete;

		// Movable:
		VmaBuffer(VmaBuffer&& other) noexcept;
		VmaBuffer& operator=(VmaBuffer&& other) noexcept;

		// Getters:
		const VkBuffer& GetVkBuffer() const;
		const VmaAllocation& GetVmaAllocation() const;
		const BufferCreateInfo& GetBufferCreateInfo() const;
		const VmaAllocationCreateInfo& GetAllocationCreateInfo() const;
		uint64_t GetSize();
		void SetDebugName(const std::string& name);

	private: // Methods:
		void Cleanup();
		void MoveFrom(VmaBuffer& other) noexcept;
	};
}