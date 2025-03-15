#ifndef __INCLUDE_GUARD_vulkanAllocationTracker_h__
#define __INCLUDE_GUARD_vulkanAllocationTracker_h__
#include <unordered_set>



namespace emberEngine
{
	// Forward declarations:
	class VmaBuffer;
	class VmaImage;



	class VulkanAllocationTracker
	{
	private: // Members:
		std::unordered_set<VmaBuffer*> m_pVmaBuffers;
		std::unordered_set<VmaImage*> m_pVmaImages;

	public: // Methods:
		VulkanAllocationTracker();
		~VulkanAllocationTracker();

		void AddVmaBuffer(VmaBuffer* vmaBuffer);
		void AddVmaImage(VmaImage* vmaImage);
		void RemoveVmaBuffer(VmaBuffer* vmaBuffer);
		void RemoveVmaImage(VmaImage* vmaImage);
	};
}



#endif // __INCLUDE_GUARD_vulkanAllocationTracker_h__