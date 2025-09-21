#pragma once
#include "vmaAllocationCreateInfo.h"
#include "vulkanBufferCreateInfo.h"
#include <string>



// Forward declarations:
typedef struct VkBuffer_T* VkBuffer;
typedef struct VmaAllocation_T* VmaAllocation;



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
		BufferCreateInfo m_bufferInfo;
		AllocationCreateInfo m_allocationInfo;

	public: // Methods:
		VmaBuffer(const std::string& name, const BufferCreateInfo& bufferInfo, const AllocationCreateInfo& allocInfo);
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
		const BufferCreateInfo& GetBufferCreateInfo() const;
		const AllocationCreateInfo& GetAllocationCreateInfo() const;
		uint64_t GetSize();

	private: // Methods:
		void Cleanup();
		void MoveFrom(VmaBuffer& other) noexcept;
	};
}