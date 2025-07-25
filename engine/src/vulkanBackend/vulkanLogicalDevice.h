#ifndef __INCLUDE_GUARD_vulkanLogicalDevice_h__
#define __INCLUDE_GUARD_vulkanLogicalDevice_h__
#include "vulkanDeviceQueue.h"
#include <vulkan/vulkan.h>
#include <vector>



namespace emberEngine
{
	namespace vulkanBackend
	{
		// Forward declarations:
		class PhysicalDevice;
		class Surface;



		class LogicalDevice
		{
		private: // Members:
			VkDevice m_device;
			DeviceQueue m_graphicsQueue;	// graphics and sync compute queue
			DeviceQueue m_presentQueue;		// present queue
			DeviceQueue m_computeQueue;		// async compute queue
			DeviceQueue m_transferQueue;	// async transfer queue

		public: // Methods:
			LogicalDevice();
			void Init(PhysicalDevice* pPhysicalDevice, Surface* pSurface, std::vector<const char*> deviceExtensions);
			~LogicalDevice();

			// Getters:
			const VkDevice& GetVkDevice() const;
			const DeviceQueue& GetGraphicsQueue() const;
			const DeviceQueue& GetPresentQueue() const;
			const DeviceQueue& GetComputeQueue() const;
			const DeviceQueue& GetTransferQueue() const;

		private: // Methods:
			std::pair<uint32_t, uint32_t> FindGraphicsComputeTransferQueueFamilyIndex(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR vkSurfaceKHR) const;
			void PrintQueueFamilyInfo(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR vkSurfaceKHR);
		};
	}
}



#endif // __INCLUDE_GUARD_vulkanLogicalDevice_h__