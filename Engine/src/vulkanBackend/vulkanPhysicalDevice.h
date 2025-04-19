#ifndef __INCLUDE_GUARD_vulkanPhysicalDevice_h__
#define __INCLUDE_GUARD_vulkanPhysicalDevice_h__
#include <vulkan/vulkan.h>



namespace emberEngine
{
	namespace vulkanBackend
	{
		// Forward declarations:
		class Instance;



		class PhysicalDevice
		{
		private: // Members:
			VkPhysicalDevice m_physicalDevice;
			VkSampleCountFlagBits m_maxMsaaSamples;
			VkBool32 m_supportsDepthClamp = false;
			VkBool32 m_supportsDepthBiasClamp = false;
			VkBool32 m_supportsMultiViewport = false;

		public: // Methods:
			PhysicalDevice(Instance* pInstance);
			~PhysicalDevice();

			const VkPhysicalDevice& GetVkPhysicalDevice() const;
			VkSampleCountFlagBits GetMaxMsaaSamples() const;
			VkBool32 SupportsDepthClamp() const;
			VkBool32 SupportsDepthBiasClamp() const;
			VkBool32 SupportsMultiViewport() const;

		private: // Methods:
			int DeviceScore(VkPhysicalDevice device);
			VkBool32 HasGraphicsAndComputeQueueFamily(VkPhysicalDevice device) const;
			VkBool32 HasPresentQueueFamily(VkPhysicalDevice device, VkSurfaceKHR surface) const;
			VkSampleCountFlagBits MaxUsableMsaaSampleCount() const;
		};
	}
}



#endif // __INCLUDE_GUARD_vulkanPhysicalDevice_h__