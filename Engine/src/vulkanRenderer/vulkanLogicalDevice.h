#ifndef __INCLUDE_GUARD_vulkanLogicalDevice_h__
#define __INCLUDE_GUARD_vulkanLogicalDevice_h__
#include "vulkanQueue.h"
#include <vulkan/vulkan.h>
#include <vector>



namespace emberEngine
{
	// Forward declarations:
	class VulkanPhysicalDevice;
	class VulkanSurface;



	class VulkanLogicalDevice
	{
	private: // Members:
		VkDevice m_device;
		VulkanQueue m_graphicsQueue;	// graphics and sync compute queue
		VulkanQueue m_presentQueue;		// present queue
		VulkanQueue m_computeQueue;		// async compute queue
		VulkanQueue m_transferQueue;	// async transfer queue

	public: // Methods:
		VulkanLogicalDevice(VulkanPhysicalDevice* pPhysicalDevice, VulkanSurface* pSurface, std::vector<const char*> deviceExtensions);
		~VulkanLogicalDevice();

		// Getters:
		const VkDevice& GetVkDevice() const;
		const VulkanQueue GetGraphicsQueue() const;
		const VulkanQueue GetPresentQueue() const;
		const VulkanQueue GetComputeQueue() const;
		const VulkanQueue GetTransferQueue() const;

	private: // Methods:
		uint32_t FindGraphicsAndComputeQueueFamilyIndex(VkPhysicalDevice vkPhysicalDevice) const;
		uint32_t FindPresentQueueFamilyIndex(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR vkSurfaceKHR) const;
		uint32_t FindPureComputeQueueFamilyIndex(VkPhysicalDevice vkPhysicalDevice) const;
		uint32_t FindPureTransferQueueFamilyIndex(VkPhysicalDevice vkPhysicalDevice) const;
	};
}



#endif // __INCLUDE_GUARD_vulkanLogicalDevice_h__