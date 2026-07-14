#pragma once
#include <unordered_set>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class LogicalDevice;



    // Defines how many descriptors per descriptorType are expected on average:
	struct DescriptorTypeCount
	{
		VkDescriptorType descriptorType;
		uint32_t descriptorCount;
	};



	class DescriptorPool
	{
	private: // Members:
		VkDescriptorPool m_descriptorPool;
		std::vector<DescriptorTypeCount> m_perSetCapacities;        // average expected distribution of descriptors within a descriptor set.
		std::unordered_set<VkDescriptorSetLayout> m_failedLayouts;  // layouts that do not fit the current free descriptor distribution.
		uint32_t m_maxSets;             // max number of descriptorSets that this pool can provide.
		uint32_t m_allocatedSetCount;   // keeps track of number of allocated descriptor sets from this pool.

	public: // Methods:
		DescriptorPool(const std::vector<DescriptorTypeCount>& perSetCapacities, uint32_t maxSets);
		~DescriptorPool();

		// Non-copyable:
		DescriptorPool(const DescriptorPool&) = delete;
		DescriptorPool& operator=(const DescriptorPool&) = delete;

		// Movable:
		DescriptorPool(DescriptorPool&& other) noexcept;
		DescriptorPool& operator=(DescriptorPool&& other) noexcept;

		// Allocation:
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