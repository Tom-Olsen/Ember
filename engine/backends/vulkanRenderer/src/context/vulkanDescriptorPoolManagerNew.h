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



	/// <summary>
	/// Handle to a descriptorSet allocated by the DescriptorPoolManager.
	/// Keep it around and pass it back to FreeDescriptorSet(...), so the manager
	/// knows which pool the set came from. pPool is opaque to the caller.
	/// </summary>
	struct DescriptorSetAllocation
	{
		VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
		DescriptorPool* pPool = nullptr;
	};



	/// <summary>
	/// Manages a growing/shrinking list of DescriptorPools of a single generic kind. <para/>
	/// All pools share the same per descriptorSet capacity ratios for the supported VkDescriptorTypes,
	/// only their maxSets count differs: each additional pool doubles maxSets (capped). <para/>
	/// Allocation walks the pool list and grabs the first pool with free capacity, adding a new pool
	/// when all are full. Freeing a descriptorSet destroys its pool once the pool runs empty
	/// (one pool is always retained to avoid churn, use TrimPools() to release everything).
	/// </summary>
	class DescriptorPoolManager
	{
	private: // Members:
		LogicalDevice* m_pLogicalDevice;
		std::vector<std::unique_ptr<DescriptorPool>> m_pools;
		uint32_t m_nextPoolMaxSets;
		uint32_t m_poolNameCounter;	// only used for unique debug names.

		static constexpr uint32_t s_initialPoolMaxSets = 64;	// maxSets of the first pool.
		static constexpr uint32_t s_maxPoolMaxSets = 2048;		// maxSets doubling cap.

	public: // Methods:
		DescriptorPoolManager(LogicalDevice* pLogicalDevice);
		~DescriptorPoolManager();

		// Non-copyable:
		DescriptorPoolManager(const DescriptorPoolManager&) = delete;
		DescriptorPoolManager& operator=(const DescriptorPoolManager&) = delete;

		// Movable:
		DescriptorPoolManager(DescriptorPoolManager&& other) noexcept;
		DescriptorPoolManager& operator=(DescriptorPoolManager&& other) noexcept;

		// Allocation:
		/// <summary>
		/// Allocates a single descriptorSet with the given layout.
		/// The debugName is only used for vulkan object naming when validation layers are active.
		/// </summary>
		DescriptorSetAllocation AllocateDescriptorSet(VkDescriptorSetLayout layout, const std::string& debugName = "descriptorSet");
		/// <summary>
		/// Allocates 'count' descriptorSets with identical layout, e.g. one per frame in flight.
		/// The sets may be spread across multiple pools.
		/// </summary>
		std::vector<DescriptorSetAllocation> AllocateDescriptorSets(uint32_t count, VkDescriptorSetLayout layout, const std::string& debugName = "descriptorSet");
		void FreeDescriptorSet(const DescriptorSetAllocation& allocation);
		void FreeDescriptorSets(const std::vector<DescriptorSetAllocation>& allocations);

		// Maintenance:
		/// <summary>
		/// Destroys all empty pools, including the one that is normally retained.
		/// </summary>
		void TrimPools();

		// Getters:
		uint32_t GetPoolCount() const;
		uint32_t GetAllocatedSetCount() const;

		// Debugging:
		std::string ToString() const;

	private: // Methods:
		DescriptorPool* AddPool();
		void Cleanup();
		void MoveFrom(DescriptorPoolManager& other) noexcept;
	};
}
