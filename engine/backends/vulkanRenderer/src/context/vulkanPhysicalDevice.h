#ifndef __INCLUDE_GUARD_vulkanRendererBackend_vulkanPhysicalDevice_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_vulkanPhysicalDevice_h__
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Instance;



	class PhysicalDevice
	{
	private: // Members:
		VkPhysicalDevice m_physicalDevice;
		VkSampleCountFlagBits m_maxMsaaSamples;
		VkBool32 m_supportsDepthClamp;
		VkBool32 m_supportsDepthBiasClamp;
		VkBool32 m_supportsMultiViewport;

	public: // Methods:
		PhysicalDevice();
		~PhysicalDevice();

		// Non-copyable:
		PhysicalDevice(const PhysicalDevice&) = delete;
		PhysicalDevice& operator=(const PhysicalDevice&) = delete;

		// Movable:
		PhysicalDevice(PhysicalDevice&& other) noexcept;
		PhysicalDevice& operator=(PhysicalDevice&& other) noexcept;

		void Init(Instance* pInstance);
		const VkPhysicalDevice& GetVkPhysicalDevice() const;
		VkSampleCountFlagBits GetMaxMsaaSamples() const;
		VkBool32 SupportsDepthClamp() const;
		VkBool32 SupportsDepthBiasClamp() const;
		VkBool32 SupportsMultiViewport() const;

	private: // Methods:
		void Cleanup();
		void MoveFrom(PhysicalDevice& other) noexcept;
		int DeviceScore(VkPhysicalDevice device);
		VkBool32 HasGraphicsAndComputeQueueFamily(VkPhysicalDevice device) const;
		VkBool32 HasPresentQueueFamily(VkPhysicalDevice device, VkSurfaceKHR surface) const;
		VkSampleCountFlagBits MaxUsableMsaaSampleCount() const;
	};
}



#endif // __INCLUDE_GUARD_vulkanRendererBackend_vulkanPhysicalDevice_h__