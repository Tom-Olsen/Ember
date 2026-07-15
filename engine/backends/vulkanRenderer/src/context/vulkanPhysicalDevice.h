#pragma once
#include <cstdint>
#include <vulkan/vulkan.h>



// Forward declarations:
typedef struct VkPhysicalDevice_T* VkPhysicalDevice;
typedef struct VkSurfaceKHR_T* VkSurfaceKHR;



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Instance;



	class PhysicalDevice
	{
    private: // Structs:
		struct DeviceCandidate
		{
			VkPhysicalDevice device;
			int score;
			bool isDiscrete;
		};

	private: // Members:
		VkPhysicalDevice m_physicalDevice;
		VkSampleCountFlagBits m_maxMsaaSamples;
		uint32_t m_maxImageDimension1d;
		uint32_t m_maxImageDimension2d;
		uint32_t m_maxImageDimension3d;

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
		VkSampleCountFlagBits GetMaxMsaaSamples() const;
		uint32_t GetMaxImageDimension1d() const;
		uint32_t GetMaxImageDimension2d() const;
		uint32_t GetMaxImageDimension3d() const;
		bool SupportsDepthClamp() const;
		bool SupportsDepthBiasClamp() const;
		bool SupportsFillModeNonSolid() const;
		bool SupportsMultiViewport() const;

	private: // Methods:
		void Cleanup();
		void MoveFrom(PhysicalDevice& other) noexcept;
		int DeviceScore(VkPhysicalDevice device);
		bool DeviceSupportsScalarBlockLayout(VkPhysicalDevice device) const;
		bool HasGraphicsAndComputeQueueFamily(VkPhysicalDevice device) const;
		bool HasPresentQueueFamily(VkPhysicalDevice device, VkSurfaceKHR surface) const;
		VkSampleCountFlagBits MaxUsableMsaaSampleCount() const;
	};
}