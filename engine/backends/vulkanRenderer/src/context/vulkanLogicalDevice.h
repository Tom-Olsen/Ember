#pragma once
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>



// Forward declarations:
typedef struct VkDevice_T* VkDevice;
typedef struct VkPhysicalDevice_T* VkPhysicalDevice;
typedef struct VkSurfaceKHR_T* VkSurfaceKHR;



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct DeviceQueue;
	class PhysicalDevice;
	class Surface;



	class LogicalDevice
	{
	private: // Members:
		VkDevice m_device = nullptr;
		std::vector<std::unique_ptr<DeviceQueue>> m_deviceQueues;
		DeviceQueue* m_pGraphicsQueue = nullptr;
		DeviceQueue* m_pPresentQueue = nullptr;
		DeviceQueue* m_pTransferQueue = nullptr;
		std::vector<DeviceQueue*> m_computeQueues;

	public: // Methods:
		// Constructor/Destructor:
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
		const DeviceQueue& GetTransferQueue() const;
		const std::vector<DeviceQueue*>& GetComputeQueues() const;

	private: // Methods:
		void Cleanup();
		void MoveFrom(LogicalDevice& other) noexcept;
		std::pair<uint32_t, uint32_t> FindMainQueueFamilyIndex(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR vkSurfaceKHR) const;
		void PrintQueueFamilyInfo(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR vkSurfaceKHR);
	};
}