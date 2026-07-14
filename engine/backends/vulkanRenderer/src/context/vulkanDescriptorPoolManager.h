#pragma once
#include "vulkanDescriptorPool.h"
#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class LogicalDevice;



	// Allocation objects, matches a descriptorSet and its parent descriptorPool:
	struct DescriptorSetAllocation
	{
		VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
		DescriptorPool* pPool = nullptr;
	};



	/// <summary>
	/// Manages a growing/shrinking list of DescriptorPools of a single generic kind. <para/>
	/// All pools share the same fixed maxSets count and per descriptorSet capacity ratios
	/// for the supported VkDescriptorTypes. <para/>
	/// Allocation walks the pool list and grabs the first pool with free capacity, adding a new pool
	/// when all are full. Freeing a descriptorSet releases manager ownership once the pool runs empty
	/// (one pool is always retained to avoid churn).
	/// </summary>
	class DescriptorPoolManager
	{
	private: // Members:
		static std::vector<std::unique_ptr<DescriptorPool>> s_pools;

	public: // Methods:
	    // Initialization/Cleanup:
        static void Init();
        static void Clear();

		// Allocation:
		static DescriptorSetAllocation AllocateDescriptorSet(VkDescriptorSetLayout layout, const std::string& debugName = "descriptorSet");
		static std::vector<DescriptorSetAllocation> AllocateDescriptorSets(uint32_t count, VkDescriptorSetLayout layout, const std::string& debugName = "descriptorSet");
		static void FreeDescriptorSet(const DescriptorSetAllocation& allocation);
		static void FreeDescriptorSets(const std::vector<DescriptorSetAllocation>& allocations);

		// Getters:
		static uint32_t GetPoolCount();
		static uint32_t GetAllocatedSetCount();

		// Debugging:
		static std::string ToString();

	private: // Methods:
		static DescriptorPool* AddPool();

        // Delete all constructors:
        DescriptorPoolManager() = delete;
        DescriptorPoolManager(const DescriptorPoolManager&) = delete;
        DescriptorPoolManager& operator=(const DescriptorPoolManager&) = delete;
        DescriptorPoolManager(DescriptorPoolManager&&) = delete;
        DescriptorPoolManager& operator=(DescriptorPoolManager&&) = delete;
        ~DescriptorPoolManager() = delete;
	};
}