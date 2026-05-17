#pragma once
#include "vk_mem_alloc.h"
#include <string>
#include <unordered_map>



// Forward declarations:
typedef struct VkBuffer_T* VkBuffer;
typedef struct VkDeviceMemory_T* VkDeviceMemory;



namespace vulkanRendererBackend
{
	// Forward declarations:
	class VmaBuffer;
	class VmaImage;



	class AllocationTracker
	{
	private: // Structs:
		struct BufferAllocationInfo
		{
			VkBuffer buffer;
			VkDeviceMemory memory;
			std::string name;
			uint64_t offset;
			uint64_t size;
		};

	private: // Members:
		std::unordered_map<VmaAllocation, BufferAllocationInfo> m_vmaBufferAllocations;
		std::unordered_map<VmaAllocation, std::string> m_vmaImageAllocations;

	public: // Methods:
		AllocationTracker();
		~AllocationTracker();

		// Non-copyable:
		AllocationTracker(const AllocationTracker&) = delete;
		AllocationTracker& operator=(const AllocationTracker&) = delete;

		// Movable:
		AllocationTracker(AllocationTracker&& other) noexcept;
		AllocationTracker& operator=(AllocationTracker&& other) noexcept;

		// Registration:
		void AddVmaBufferAllocation(VkBuffer buffer, VmaAllocation allocation, const std::string& name);
		void AddVmaImageAllocation(VmaAllocation allocation, const std::string& name);
		void UpdateVmaBufferAllocationName(VmaAllocation allocation, const std::string& name);
		void RemoveVmaBufferAllocation(VmaAllocation allocation);
		void RemoveVmaImageAllocation(VmaAllocation allocation);
		void DumpVmaBufferAllocations() const;
		void DumpVmaImageAllocations() const;

	private: // Methods:
		void Cleanup();
		void MoveFrom(AllocationTracker& other) noexcept;
	};
}