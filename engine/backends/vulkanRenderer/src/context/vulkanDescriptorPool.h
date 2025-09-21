#pragma once



// Forward declarations:
typedef struct VkDescriptorPool_T* VkDescriptorPool;



namespace vulkanRendererBackend
{
	// Forward declarations:
	class LogicalDevice;



	class DescriptorPool
	{
	private: // Members:
		VkDescriptorPool m_descriptorPool;
		LogicalDevice* m_pLogicalDevice;

	public: // Methods:
		DescriptorPool(LogicalDevice* pLogicalDevice);
		~DescriptorPool();

		// Non-copyable:
		DescriptorPool(const DescriptorPool&) = delete;
		DescriptorPool& operator=(const DescriptorPool&) = delete;

		// Movable:
		DescriptorPool(DescriptorPool&& other) noexcept;
		DescriptorPool& operator=(DescriptorPool&& other) noexcept;

		const VkDescriptorPool& GetVkDescriptorPool() const;

	private: // Methods:
		void Cleanup();
		void MoveFrom(DescriptorPool& other) noexcept;
	};
}