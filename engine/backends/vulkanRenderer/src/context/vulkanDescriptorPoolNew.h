#pragma once
#include <string>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class LogicalDevice;



	/// <summary>
	/// Number of descriptors of a single VkDescriptorType.
	/// </summary>
	struct DescriptorTypeCount
	{
		VkDescriptorType descriptorType;
		uint32_t descriptorCount;
	};



	/// <summary>
	/// Wrapper for a single fixed size VkDescriptorPool.
	/// The pool is sized for 'maxSets' descriptor sets, where each set may consume
	/// up to 'perSetCapacities' descriptors of each listed type.
	/// Tracks the number of live descriptor sets so the owning DescriptorPoolManager
	/// can grow and shrink its pool list. Managed by DescriptorPoolManager.
	/// </summary>
	class DescriptorPool
	{
	private: // Members:
		VkDescriptorPool m_descriptorPool;
		LogicalDevice* m_pLogicalDevice;
		std::vector<DescriptorTypeCount> m_perSetCapacities;
		uint32_t m_maxSets;
		uint32_t m_allocatedSetCount;
		bool m_exhausted;	// set when an allocation fails despite free set slots (pool fragmentation), cleared on free.

	public: // Methods:
		DescriptorPool(LogicalDevice* pLogicalDevice, const std::vector<DescriptorTypeCount>& perSetCapacities, uint32_t maxSets, const std::string& name);
		~DescriptorPool();

		// Non-copyable:
		DescriptorPool(const DescriptorPool&) = delete;
		DescriptorPool& operator=(const DescriptorPool&) = delete;

		// Movable:
		DescriptorPool(DescriptorPool&& other) noexcept;
		DescriptorPool& operator=(DescriptorPool&& other) noexcept;

		// Allocation:
		/// <summary>
		/// Tries to allocate a single descriptor set with the given layout.
		/// Returns VK_NULL_HANDLE when the pool is full or fragmented.
		/// </summary>
		VkDescriptorSet TryAllocateDescriptorSet(VkDescriptorSetLayout layout);
		void FreeDescriptorSet(VkDescriptorSet descriptorSet);

		// Getters:
		const VkDescriptorPool& GetVkDescriptorPool() const;
		const std::vector<DescriptorTypeCount>& GetPerSetCapacities() const;
		uint32_t GetMaxSets() const;
		uint32_t GetAllocatedSetCount() const;
		bool IsFull() const;
		bool IsEmpty() const;

	private: // Methods:
		void Cleanup();
		void MoveFrom(DescriptorPool& other) noexcept;
	};
}