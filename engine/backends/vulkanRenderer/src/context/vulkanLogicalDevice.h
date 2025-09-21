#pragma once
#include "vulkanDeviceQueue.h"
#include <vector>



// Forward declarations:
typedef struct VkDevice_T* VkDevice;
typedef struct VkPhysicalDevice_T* VkPhysicalDevice;
typedef struct VkSurfaceKHR_T* VkSurfaceKHR;



namespace vulkanRendererBackend
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
		LogicalDevice(PhysicalDevice* pPhysicalDevice, Surface* pSurface, std::vector<const char*>& deviceExtensions);
		~LogicalDevice();

		// Non-copyable:
		LogicalDevice(const LogicalDevice&) = delete;
		LogicalDevice& operator=(const LogicalDevice&) = delete;

		// Movable:
		LogicalDevice(LogicalDevice&& other) noexcept;
		LogicalDevice& operator=(LogicalDevice&& other) noexcept;

		// Getters:
		const VkDevice& GetVkDevice() const;
		const DeviceQueue& GetGraphicsQueue() const;
		const DeviceQueue& GetPresentQueue() const;
		const DeviceQueue& GetComputeQueue() const;
		const DeviceQueue& GetTransferQueue() const;

	private: // Methods:
		void Cleanup();
		void MoveFrom(LogicalDevice& other) noexcept;
		std::pair<uint32_t, uint32_t> FindGraphicsComputeTransferQueueFamilyIndex(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR vkSurfaceKHR) const;
		void PrintQueueFamilyInfo(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR vkSurfaceKHR);
	};
}