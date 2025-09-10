#ifndef __INCLUDE_GUARD_vulkanRendererBackend_vulkanMemoryAllocator_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_vulkanMemoryAllocator_h__
#include "vk_mem_alloc.h"



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Instance;
	class LogicalDevice;
	class PhysicalDevice;



	class MemoryAllocator
	{
	private: // Members:
		VmaAllocator m_pAllocator;

	public: // Methods:
		MemoryAllocator();
		~MemoryAllocator();

		// Non-copyable:
		MemoryAllocator(const MemoryAllocator&) = delete;
		MemoryAllocator& operator=(const MemoryAllocator&) = delete;

		// Movable:
		MemoryAllocator(MemoryAllocator&& other) noexcept;
		MemoryAllocator& operator=(MemoryAllocator&& other) noexcept;

		void Init(Instance* pInstance, LogicalDevice* pLogicalDevice, PhysicalDevice* pPhysicalDevice);
		const VmaAllocator& GetVmaAllocator() const;

	private: // Methods:
		void Cleanup();
		void MoveFrom(MemoryAllocator& other) noexcept;
	};
}



#endif // __INCLUDE_GUARD_vulkanRendererBackend_vulkanMemoryAllocator_h__