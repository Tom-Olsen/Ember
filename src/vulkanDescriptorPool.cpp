#include "vulkanDescriptorPool.h"
#include "macros.h"
#include <iostream>



// Constructor:
VulkanDescriptorPool::VulkanDescriptorPool(VulkanContext* context)
{
	this->context = context;


	uint32_t maxSets = 100;	// maximum number of descriptor sets that may be allocated
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
	poolInfo.maxSets = maxSets;

	VKA(vkCreateDescriptorPool(context->LogicalDevice(), &poolInfo, nullptr, &descriptorPool));
}



// Destructor:
VulkanDescriptorPool::~VulkanDescriptorPool()
{
	vkDestroyDescriptorPool(context->LogicalDevice(), descriptorPool, nullptr);
}