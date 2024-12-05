#include "vulkanDescriptorPool.h"
#include "vulkanMacros.h"
#include <iostream>



// Constructor:
VulkanDescriptorPool::VulkanDescriptorPool(VulkanLogicalDevice* logicalDevice)
{
	this->logicalDevice = logicalDevice;


	uint32_t maxSets = 20;	// maximum number of descriptor sets that may be allocated
	std::array<VkDescriptorPoolSize, 4> poolSizes
	{
		VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, maxSets },
		VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, maxSets },
		VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_SAMPLER, maxSets },
		VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, maxSets }
		// Add more descriptor types as needed
	};

	VkDescriptorPoolCreateInfo poolInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = 10000;

	VKA(vkCreateDescriptorPool(logicalDevice->device, &poolInfo, nullptr, &descriptorPool));
}



// Destructor:
VulkanDescriptorPool::~VulkanDescriptorPool()
{
	vkDestroyDescriptorPool(logicalDevice->device, descriptorPool, nullptr);
}