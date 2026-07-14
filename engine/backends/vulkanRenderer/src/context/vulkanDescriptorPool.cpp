#include "vulkanDescriptorPool.h"
#include "vulkanGarbageCollector.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include <assert.h>
#include <string>
#include <utility>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Constructor/Destructor:
	DescriptorPool::DescriptorPool(LogicalDevice* pLogicalDevice, const std::vector<DescriptorTypeCount>& perSetCapacities, uint32_t maxSets)
	{
		// Assertions:
		assert(pLogicalDevice != nullptr);
		assert(!perSetCapacities.empty());
		assert(maxSets > 0);

		m_pLogicalDevice = pLogicalDevice;
		m_perSetCapacities = perSetCapacities;
		m_maxSets = maxSets;
		m_allocatedSetCount = 0;

		// Total pool capacity = per set capacity * maxSets for each descriptor type:
		std::vector<VkDescriptorPoolSize> poolSizes;
		poolSizes.reserve(m_perSetCapacities.size());
		for (const DescriptorTypeCount& typeCount : m_perSetCapacities)
			poolSizes.push_back(VkDescriptorPoolSize{ typeCount.descriptorType, typeCount.descriptorCount * maxSets });

		VkDescriptorPoolCreateInfo poolInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;	// allows for individual descriptorSet deletion.
		poolInfo.maxSets = maxSets;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();

		VKA(vkCreateDescriptorPool(m_pLogicalDevice->GetVkDevice(), &poolInfo, nullptr, &m_descriptorPool));
        static int poolCounter = 0;
		NAME_VK_OBJECT(m_descriptorPool, "DescriptorPool_#" + std::to_string(poolCounter));
        poolCounter++;
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



	// Allocation:
	VkDescriptorSet DescriptorPool::TryAllocateDescriptorSet(VkDescriptorSetLayout layout)
	{
		if (IsFull())
			return VK_NULL_HANDLE;
		if (m_failedLayouts.find(layout) != m_failedLayouts.end())
			return VK_NULL_HANDLE;

		VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		allocInfo.descriptorPool = m_descriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &layout;

		VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
		VkResult result = vkAllocateDescriptorSets(m_pLogicalDevice->GetVkDevice(), &allocInfo, &descriptorSet);
		if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL)
		{
			m_failedLayouts.insert(layout);
			return VK_NULL_HANDLE;
		}
		VULKAN_ASSERT(result);

		m_allocatedSetCount++;
		return descriptorSet;
	}
	void DescriptorPool::FreeDescriptorSet(VkDescriptorSet descriptorSet)
	{
		assert(m_allocatedSetCount > 0);
        VKA(vkFreeDescriptorSets(m_pLogicalDevice->GetVkDevice(), m_descriptorPool, 1, &descriptorSet));
		m_failedLayouts.clear();
		m_allocatedSetCount--;
	}



	// Getters:
	const VkDescriptorPool& DescriptorPool::GetVkDescriptorPool() const
	{
		return m_descriptorPool;
	}
	const std::vector<DescriptorTypeCount>& DescriptorPool::GetPerSetCapacities() const
	{
		return m_perSetCapacities;
	}
	uint32_t DescriptorPool::GetMaxSets() const
	{
		return m_maxSets;
	}
	uint32_t DescriptorPool::GetAllocatedSetCount() const
	{
		return m_allocatedSetCount;
	}
	bool DescriptorPool::IsFull() const
	{
		return m_allocatedSetCount >= m_maxSets;
	}
	bool DescriptorPool::IsEmpty() const
	{
		return m_allocatedSetCount == 0;
	}



	// Private methods:
	void DescriptorPool::Cleanup()
	{
		if (m_pLogicalDevice == nullptr || m_descriptorPool == VK_NULL_HANDLE)
			return;

        // vkDescriptorSets are disposed by garbage collector and pool must be deleted after all its sets.
        // => pool must also be disposed by garbage collector:
		VkDevice device = m_pLogicalDevice->GetVkDevice();
		VkDescriptorPool descriptorPool = m_descriptorPool;
		m_descriptorPool = VK_NULL_HANDLE;
		GarbageCollector::RecordGarbage([device, descriptorPool]()
		{
			vkDestroyDescriptorPool(device, descriptorPool, nullptr);
		});
	}
	void DescriptorPool::MoveFrom(DescriptorPool& other) noexcept
	{
		m_descriptorPool = other.m_descriptorPool;
		m_pLogicalDevice = other.m_pLogicalDevice;
		m_perSetCapacities = std::move(other.m_perSetCapacities);
		m_failedLayouts = std::move(other.m_failedLayouts);
		m_maxSets = other.m_maxSets;
		m_allocatedSetCount = other.m_allocatedSetCount;

		other.m_descriptorPool = VK_NULL_HANDLE;
		other.m_pLogicalDevice = nullptr;
		other.m_maxSets = 0;
		other.m_allocatedSetCount = 0;
	}
}