#include "vulkanDescriptorPool.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include <array>
#include <assert.h>



namespace vulkanRendererBackend
{
	// Constructor/Destructor:
	DescriptorPool::DescriptorPool()
	{
		m_descriptorPool = VK_NULL_HANDLE;
		m_pLogicalDevice = nullptr;
	}
	DescriptorPool::~DescriptorPool()
	{
		Cleanup();
	}


	// Move semantics:
	DescriptorPool::DescriptorPool(DescriptorPool&& other) noexcept
	{
		MoveFrom(other);
	}
	DescriptorPool& DescriptorPool::operator=(DescriptorPool&& other) noexcept
	{
		if (this != &other)
		{
			Cleanup();
			MoveFrom(other);
		}
		return *this;
	}



	// Public methods:
	void DescriptorPool::Init(LogicalDevice* pLogicalDevice)
	{
		// Assertions:
		assert(pLogicalDevice != nullptr);

		m_pLogicalDevice = pLogicalDevice;

		// Ember::ToDo: create dynamic way to add more descirptors if needed. See that one great webpage (Link?) fr guidance.
		uint32_t descriptorCount = 8192;	// maximum number of descriptor of each type in the associated pool
		std::array<VkDescriptorPoolSize, 6> poolSizes
		{
			VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, descriptorCount },
			VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, descriptorCount },
			VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_SAMPLER, descriptorCount },
			VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, descriptorCount },
			VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, descriptorCount },
			VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, descriptorCount }
			// Add more descriptor types as needed
		};

		VkDescriptorPoolCreateInfo poolInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;	// allows for individual descriptoSet deletion.
		poolInfo.maxSets = descriptorCount * poolSizes.size();
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();

		VKA(vkCreateDescriptorPool(m_pLogicalDevice->GetVkDevice(), &poolInfo, nullptr, &m_descriptorPool));
	}
	const VkDescriptorPool& DescriptorPool::GetVkDescriptorPool() const
	{
		return m_descriptorPool;
	}



	// Private methods:
	void DescriptorPool::Cleanup()
	{
		vkDestroyDescriptorPool(m_pLogicalDevice->GetVkDevice(), m_descriptorPool, nullptr);
	}
	void DescriptorPool::MoveFrom(DescriptorPool& other) noexcept
	{
		m_descriptorPool = other.m_descriptorPool;
		m_pLogicalDevice = other.m_pLogicalDevice;

		other.m_descriptorPool = VK_NULL_HANDLE;
		other.m_pLogicalDevice = nullptr;
	}
}