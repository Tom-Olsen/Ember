#include "vulkanDescriptorPoolManager.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include <algorithm>
#include <assert.h>
#include <sstream>
#include <stdexcept>



namespace vulkanRendererBackend
{
    // Static members:
	std::vector<std::unique_ptr<DescriptorPool>> DescriptorPoolManager::s_pools;




    // Initialization/Cleanup:
    void DescriptorPoolManager::Init()
    {

    }
    void DescriptorPoolManager::Clear()
    {
		s_pools.clear();
    }



	// Allocation:
	DescriptorSetAllocation DescriptorPoolManager::AllocateDescriptorSet(VkDescriptorSetLayout layout, const std::string& debugName)
	{
		// Try existing pools:
		DescriptorSetAllocation allocation;
		for (std::unique_ptr<DescriptorPool>& pool : s_pools)
		{
			allocation.descriptorSet = pool->TryAllocateDescriptorSet(layout);
			if (allocation.descriptorSet != VK_NULL_HANDLE)
			{
				allocation.pPool = pool.get();
				break;
			}
		}

		// All pools rejected the layout => grow by adding a new pool:
		if (allocation.descriptorSet == VK_NULL_HANDLE)
		{
			allocation.pPool = AddPool();
			allocation.descriptorSet = allocation.pPool->TryAllocateDescriptorSet(layout);
			if (allocation.descriptorSet == VK_NULL_HANDLE)
				throw std::runtime_error("DescriptorPoolManager::AllocateDescriptorSet() failed on a fresh pool. The descriptor set layout exceeds fixed pool capacity or uses an unsupported descriptor type.");
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
		assert(allocation.descriptorSet != VK_NULL_HANDLE);
        assert(allocation.pPool != nullptr);

		allocation.pPool->FreeDescriptorSet(allocation.descriptorSet);

		// Shrink: release empty pools. Retain one pool to avoid create/destroy churn:
		if (allocation.pPool->IsEmpty() && s_pools.size() > 1)
		{
			auto poolIt = std::find_if(s_pools.begin(), s_pools.end(), [&allocation](const std::unique_ptr<DescriptorPool>& pool) { return pool.get() == allocation.pPool; });
			if (poolIt != s_pools.end())
				s_pools.erase(poolIt);
		}
	}
	void DescriptorPoolManager::FreeDescriptorSets(const std::vector<DescriptorSetAllocation>& allocations)
	{
		for (const DescriptorSetAllocation& allocation : allocations)
			FreeDescriptorSet(allocation);
	}

	// Getters:
	uint32_t DescriptorPoolManager::GetPoolCount()
	{
		return static_cast<uint32_t>(s_pools.size());
	}
	uint32_t DescriptorPoolManager::GetAllocatedSetCount()
	{
		uint32_t count = 0;
		for (const std::unique_ptr<DescriptorPool>& pool : s_pools)
			count += pool->GetAllocatedSetCount();
		return count;
	}



	// Debugging:
	std::string DescriptorPoolManager::ToString()
	{
		std::ostringstream stream;
		stream << "DescriptorPoolManager: " << s_pools.size() << " pools, " << GetAllocatedSetCount() << " live sets\n";
		for (const std::unique_ptr<DescriptorPool>& pool : s_pools)
			stream << "  pool: " << pool->GetAllocatedSetCount() << "/" << pool->GetMaxSets() << " sets" << (pool->IsFull() ? " (full)" : "") << "\n";
		return stream.str();
	}



	// Private methods:
	DescriptorPool* DescriptorPoolManager::AddPool()
	{
		constexpr uint32_t poolMaxSets = 2048;

		// Average descriptor consumption per descriptorSet.
		// Heavyweight sets may exceed these values. The pool then rejects that layout until descriptor sets are freed.
		static const std::vector<DescriptorTypeCount> s_perSetCapacities =
		{
			DescriptorTypeCount{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 4 },
			DescriptorTypeCount{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4 },
			DescriptorTypeCount{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4 },
			DescriptorTypeCount{ VK_DESCRIPTOR_TYPE_SAMPLER, 2 },
			DescriptorTypeCount{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 4 },
			DescriptorTypeCount{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 2 }
			// Add more descriptor types as needed.
		};
		s_pools.push_back(std::make_unique<DescriptorPool>(Context::GetLogicalDevice(), s_perSetCapacities, poolMaxSets));
		return s_pools.back().get();
	}
}