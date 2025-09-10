#ifndef __INCLUDE_GUARD_vulkanRendererBackend_vulkanDescriptorPool_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_vulkanDescriptorPool_h__
#include <vulkan/vulkan.h>



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
		DescriptorPool();
		~DescriptorPool();

		// Non-copyable:
		DescriptorPool(const DescriptorPool&) = delete;
		DescriptorPool& operator=(const DescriptorPool&) = delete;

		// Movable:
		DescriptorPool(DescriptorPool&& other) noexcept;
		DescriptorPool& operator=(DescriptorPool&& other) noexcept;

		void Init(LogicalDevice* pLogicalDevice);
		const VkDescriptorPool& GetVkDescriptorPool() const;

	private: // Methods:
		void Cleanup();
		void MoveFrom(DescriptorPool& other) noexcept;
	};
}



#endif // __INCLUDE_GUARD_vulkanRendererBackend_vulkanDescriptorPool_h__