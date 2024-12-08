#ifndef __INCLUDE_GUARD_vulkanMemoryAllocator_h__
#define __INCLUDE_GUARD_vulkanMemoryAllocator_h__
#include "vk_mem_alloc.h"



class VulkanInstance;
class VulkanLogicalDevice;
class VulkanPhysicalDevice;



class VulkanMemoryAllocator
{
private: // Members:
	VmaAllocator m_pAllocator;

public: // Methods:
	VulkanMemoryAllocator(VulkanInstance* pInstance, VulkanLogicalDevice* pLogicalDevice, VulkanPhysicalDevice* pPhysicalDevice);
	~VulkanMemoryAllocator();
	const VmaAllocator& GetVmaAllocator() const;
};



#endif // __INCLUDE_GUARD_vulkanMemoryAllocator_h__