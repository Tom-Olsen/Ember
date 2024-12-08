#ifndef __INCLUDE_GUARD_vulkanDescriptorPool_h__
#define __INCLUDE_GUARD_vulkanDescriptorPool_h__
#include <vulkan/vulkan.h>



class VulkanLogicalDevice;



class VulkanDescriptorPool
{
private: // Members:
	VkDescriptorPool m_descriptorPool;
	VulkanLogicalDevice* m_pLogicalDevice;

public: // Methods:
	VulkanDescriptorPool(VulkanLogicalDevice* pLogicalDevice);
	~VulkanDescriptorPool();
	const VkDescriptorPool& GetVkDescriptorPool() const;
};



#endif // __INCLUDE_GUARD_vulkanDescriptorPool_h__