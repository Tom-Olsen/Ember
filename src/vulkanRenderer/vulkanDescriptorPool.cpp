#include "vulkanDescriptorPool.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"



namespace emberEngine
{
	// Constructor/Destructor:
	VulkanDescriptorPool::VulkanDescriptorPool(VulkanLogicalDevice* pLogicalDevice)
	{
		m_pLogicalDevice = pLogicalDevice;

		uint32_t descriptorCount = 1000;	// maximum number of descriptor of each type in the associated pool
		std::array<VkDescriptorPoolSize, 4> poolSizes
		{
			VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, descriptorCount },
			VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, descriptorCount },
			VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_SAMPLER, descriptorCount },
			VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, descriptorCount }
			// Add more descriptor types as needed
		};

		VkDescriptorPoolCreateInfo poolInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
		poolInfo.maxSets = 1000 * poolSizes.size();
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();

		VKA(vkCreateDescriptorPool(m_pLogicalDevice->GetVkDevice(), &poolInfo, nullptr, &m_descriptorPool));
	}
	VulkanDescriptorPool::~VulkanDescriptorPool()
	{
		vkDestroyDescriptorPool(m_pLogicalDevice->GetVkDevice(), m_descriptorPool, nullptr);
	}



	// Public methods:
	const VkDescriptorPool& VulkanDescriptorPool::GetVkDescriptorPool() const
	{
		return m_descriptorPool;
	}
}