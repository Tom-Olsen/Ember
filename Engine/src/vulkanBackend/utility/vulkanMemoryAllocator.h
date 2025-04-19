#ifndef __INCLUDE_GUARD_vulkanMemoryAllocator_h__
#define __INCLUDE_GUARD_vulkanMemoryAllocator_h__
#include "vk_mem_alloc.h"



namespace emberEngine
{
	namespace vulkanBackend
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
			MemoryAllocator(Instance* pInstance, LogicalDevice* pLogicalDevice, PhysicalDevice* pPhysicalDevice);
			~MemoryAllocator();
			const VmaAllocator& GetVmaAllocator() const;
		};
	}
}



#endif // __INCLUDE_GUARD_vulkanMemoryAllocator_h__