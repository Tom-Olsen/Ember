#pragma once
#include "vk_mem_alloc.h"
#include <string>
#include <unordered_map>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class VmaBuffer;
	class VmaImage;



	class AllocationTracker
	{
	private: // Members:
		std::unordered_map<VmaAllocation, std::string> m_vmaBufferAllocations;
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
		void AddVmaBufferAllocation(VmaAllocation allocation, const std::string& name);
		void AddVmaImageAllocation(VmaAllocation allocation, const std::string& name);
		void RemoveVmaBufferAllocation(VmaAllocation allocation);
		void RemoveVmaImageAllocation(VmaAllocation allocation);

	private: // Methods:
		void Cleanup();
		void MoveFrom(AllocationTracker& other) noexcept;
	};
}