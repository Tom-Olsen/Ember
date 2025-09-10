#ifndef __INCLUDE_GUARD_vulkanRendererBackend_vulkanAllocationTracker_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_vulkanAllocationTracker_h__
#include <unordered_set>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class VmaBuffer;
	class VmaImage;



	class AllocationTracker
	{
	private: // Members:
		std::unordered_set<VmaBuffer*> m_pVmaBuffers;
		std::unordered_set<VmaImage*> m_pVmaImages;

	public: // Methods:
		AllocationTracker();
		~AllocationTracker();

		// Non-copyable:
		AllocationTracker(const AllocationTracker&) = delete;
		AllocationTracker& operator=(const AllocationTracker&) = delete;

		// Movable:
		AllocationTracker(AllocationTracker&& other) noexcept;
		AllocationTracker& operator=(AllocationTracker&& other) noexcept;

		void Init();
		void AddVmaBuffer(VmaBuffer* vmaBuffer);
		void AddVmaImage(VmaImage* vmaImage);
		void RemoveVmaBuffer(VmaBuffer* vmaBuffer);
		void RemoveVmaImage(VmaImage* vmaImage);

	private: // Methods:
		void Cleanup();
		void MoveFrom(AllocationTracker& other) noexcept;
	};
}



#endif // __INCLUDE_GUARD_vulkanRendererBackend_vulkanAllocationTracker_h__