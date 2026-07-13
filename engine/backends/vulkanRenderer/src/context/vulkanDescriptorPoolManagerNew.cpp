#include "vulkanDescriptorPoolManager.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include <algorithm>
#include <assert.h>
#include <sstream>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Average descriptor consumption per descriptorSet, scaled by maxSets to size each pool.
	// Heavyweight sets may exceed these ratios; the pool then reports itself full early and the manager moves on to the next pool.
	static const std::vector<DescriptorTypeCount> s_perSetCapacities =
	{
		DescriptorTypeCount{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 4 },
		DescriptorTypeCount{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4 },
		DescriptorTypeCount{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4 },
		DescriptorTypeCount{ VK_DESCRIPTOR_TYPE_SAMPLER, 2 },
		DescriptorTypeCount{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 4 },
		DescriptorTypeCount{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 2 }
		// Add more descriptor types as needed
	};



	// Constructor/Destructor:
	DescriptorPoolManager::DescriptorPoolManager(LogicalDevice* pLogicalDevice)
	{
		// Assertions:
		assert(pLogicalDevice != nullptr);

		m_pLogicalDevice = pLogicalDevice;
		m_nextPoolMaxSets = s_initialPoolMaxSets;
		m_poolNameCounter = 0;
	}
	DescriptorPoolManager::~DescriptorPoolManager()
	{
		Cleanup();
	}


	// Move semantics:
	DescriptorPoolManager::DescriptorPoolManager(DescriptorPoolManager&& other) noexcept
	{
		MoveFrom(other);
	}
	DescriptorPoolManager& DescriptorPoolManager::operator=(DescriptorPoolManager&& other) noexcept
	{
		if (this != &other)
		{
			Cleanup();
			MoveFrom(other);
		}
		return *this;
	}



	// Allocation:
	DescriptorSetAllocation DescriptorPoolManager::AllocateDescriptorSet(VkDescriptorSetLayout layout, const std::string& debugName)
	{
		// Try existing pools:
		DescriptorSetAllocation allocation;
		for (std::unique_ptr<DescriptorPool>& pool : m_pools)
		{
			allocation.descriptorSet = pool->TryAllocateDescriptorSet(layout);
			if (allocation.descriptorSet != VK_NULL_HANDLE)
			{
				allocation.pPool = pool.get();
				break;
			}
		}

		// All pools full => grow by adding a new pool:
		if (allocation.descriptorSet == VK_NULL_HANDLE)
		{
			allocation.pPool = AddPool();
			allocation.descriptorSet = allocation.pPool->TryAllocateDescriptorSet(layout);
			assert(allocation.descriptorSet != VK_NULL_HANDLE);	// layout exceeds the capacity of an entire fresh pool => s_perSetCapacities needs adjusting.
		}

		NAME_VK_OBJECT(allocation.descriptorSet, debugName);
		return allocation;
	}
	std::vector<DescriptorSetAllocation> DescriptorPoolManager::AllocateDescriptorSets(uint32_t count, VkDescriptorSetLayout layout, const std::string& debugName)
	{
		std::vector<DescriptorSetAllocation> allocations;
		allocations.reserve(count);
		for (uint32_t i = 0; i < count; i++)
			allocations.push_back(AllocateDescriptorSet(layout, debugName + std::to_string(i)));
		return allocations;
	}
	void DescriptorPoolManager::FreeDescriptorSet(const DescriptorSetAllocation& allocation)
	{
		assert(allocation.descriptorSet != VK_NULL_HANDLE && allocation.pPool != nullptr);
		if (allocation.descriptorSet == VK_NULL_HANDLE || allocation.pPool == nullptr)
			return;

		allocation.pPool->FreeDescriptorSet(allocation.descriptorSet);

		// Shrink: destroy empty pools, but always retain one pool to avoid create/destroy churn:
		if (allocation.pPool->IsEmpty() && m_pools.size() > 1)
		{
			// Reuse the freed pool size for the next growth step instead of doubling further:
			m_nextPoolMaxSets = allocation.pPool->GetMaxSets();
			auto poolIt = std::find_if(m_pools.begin(), m_pools.end(), [&allocation](const std::unique_ptr<DescriptorPool>& pool) { return pool.get() == allocation.pPool; });
			assert(poolIt != m_pools.end());	// allocation does not belong to this manager.
			m_pools.erase(poolIt);
		}
	}
	void DescriptorPoolManager::FreeDescriptorSets(const std::vector<DescriptorSetAllocation>& allocations)
	{
		for (const DescriptorSetAllocation& allocation : allocations)
			FreeDescriptorSet(allocation);
	}



	// Maintenance:
	void DescriptorPoolManager::TrimPools()
	{
		auto removeIt = std::remove_if(m_pools.begin(), m_pools.end(), [](const std::unique_ptr<DescriptorPool>& pool) { return pool->IsEmpty(); });
		m_pools.erase(removeIt, m_pools.end());

		// Continue growth from the largest surviving pool, or start over when all pools are gone:
		m_nextPoolMaxSets = s_initialPoolMaxSets;
		for (const std::unique_ptr<DescriptorPool>& pool : m_pools)
			m_nextPoolMaxSets = std::max(m_nextPoolMaxSets, pool->GetMaxSets());
	}



	// Getters:
	uint32_t DescriptorPoolManager::GetPoolCount() const
	{
		return static_cast<uint32_t>(m_pools.size());
	}
	uint32_t DescriptorPoolManager::GetAllocatedSetCount() const
	{
		uint32_t count = 0;
		for (const std::unique_ptr<DescriptorPool>& pool : m_pools)
			count += pool->GetAllocatedSetCount();
		return count;
	}



	// Debugging:
	std::string DescriptorPoolManager::ToString() const
	{
		std::ostringstream stream;
		stream << "DescriptorPoolManager: " << m_pools.size() << " pools, " << GetAllocatedSetCount() << " live sets\n";
		for (const std::unique_ptr<DescriptorPool>& pool : m_pools)
			stream << "  pool: " << pool->GetAllocatedSetCount() << "/" << pool->GetMaxSets() << " sets" << (pool->IsFull() ? " (full)" : "") << "\n";
		return stream.str();
	}



	// Private methods:
	DescriptorPool* DescriptorPoolManager::AddPool()
	{
		std::string name = "DescriptorPool_#" + std::to_string(m_poolNameCounter++);
		m_pools.push_back(std::make_unique<DescriptorPool>(m_pLogicalDevice, s_perSetCapacities, m_nextPoolMaxSets, name));
		m_nextPoolMaxSets = std::min(2 * m_nextPoolMaxSets, s_maxPoolMaxSets);
		return m_pools.back().get();
	}
	void DescriptorPoolManager::Cleanup()
	{
		assert(GetAllocatedSetCount() == 0);	// all descriptorSets should be freed before the manager dies.
		m_pools.clear();	// destroys all pools.
		m_pLogicalDevice = nullptr;
	}
	void DescriptorPoolManager::MoveFrom(DescriptorPoolManager& other) noexcept
	{
		m_pLogicalDevice = other.m_pLogicalDevice;
		m_pools = std::move(other.m_pools);
		m_nextPoolMaxSets = other.m_nextPoolMaxSets;
		m_poolNameCounter = other.m_poolNameCounter;

		other.m_pLogicalDevice = nullptr;
		other.m_pools.clear();
		other.m_nextPoolMaxSets = s_initialPoolMaxSets;
		other.m_poolNameCounter = 0;
	}
}
