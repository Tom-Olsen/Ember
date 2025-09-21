#pragma once
#include <cstdint>



// Forward declarations:
typedef struct VkPhysicalDevice_T* VkPhysicalDevice;
typedef struct VkSurfaceKHR_T* VkSurfaceKHR;



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Instance;



	class PhysicalDevice
	{
	private: // Members:
		VkPhysicalDevice m_physicalDevice;
		uint32_t m_maxMsaaSamples;
		bool m_supportsDepthClamp;
		bool m_supportsDepthBiasClamp;
		bool m_supportsMultiViewport;

	public: // Methods:
		PhysicalDevice(Instance* pInstance);
		~PhysicalDevice();

		// Non-copyable:
		PhysicalDevice(const PhysicalDevice&) = delete;
		PhysicalDevice& operator=(const PhysicalDevice&) = delete;

		// Movable:
		PhysicalDevice(PhysicalDevice&& other) noexcept;
		PhysicalDevice& operator=(PhysicalDevice&& other) noexcept;
		const VkPhysicalDevice& GetVkPhysicalDevice() const;
		uint32_t GetMaxMsaaSamples() const;
		bool SupportsDepthClamp() const;
		bool SupportsDepthBiasClamp() const;
		bool SupportsMultiViewport() const;

	private: // Methods:
		void Cleanup();
		void MoveFrom(PhysicalDevice& other) noexcept;
		int DeviceScore(VkPhysicalDevice device);
		bool HasGraphicsAndComputeQueueFamily(VkPhysicalDevice device) const;
		bool HasPresentQueueFamily(VkPhysicalDevice device, VkSurfaceKHR surface) const;
		uint32_t MaxUsableMsaaSampleCount() const;
	};
}