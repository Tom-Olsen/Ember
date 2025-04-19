#ifndef __INCLUDE_GUARD_vulkanSurface_h__
#define __INCLUDE_GUARD_vulkanSurface_h__
#include <vulkan/vulkan.h>
#include <vector>



namespace emberEngine
{
	namespace vulkanBackend
	{
		// Forward declarations:
		class Instance;
		class PhysicalDevice;
		class SdlWindow;



		class Surface
		{
		private: // Members:
			VkSurfaceKHR m_surface;
			VkSurfaceFormatKHR m_surfaceFormat;
			VkPresentModeKHR m_presentMode;
			VkSurfaceCapabilitiesKHR m_surfaceCapabilities;
			std::vector<VkSurfaceFormatKHR> m_availableSurfaceFormats;
			std::vector<VkPresentModeKHR> m_availablePresentModes;
			Instance* m_pInstance;
			PhysicalDevice* m_pPhysicalDevice;
			SdlWindow* m_pWindow;

		public: // Methods:
			Surface(Instance* pInstance, PhysicalDevice* pPhysicalDevice, SdlWindow* pWindow, bool vSyncEnabled);
			~Surface();

			// Getters:
			const VkSurfaceKHR& GetVkSurfaceKHR() const;
			const VkSurfaceFormatKHR& GetVkSurfaceFormatKHR() const;
			const VkPresentModeKHR& GetVkPresentModeKHR() const;
			VkExtent2D GetCurrentExtent();
			VkExtent2D GetMinImageExtent();
			VkExtent2D GetMaxImageExtent();
			uint32_t GetMinImageCount();
			uint32_t GetMaxImageCount();

		private: // Methods:
			VkSurfaceFormatKHR PickSurfaceFormat() const;
			VkPresentModeKHR PickPresentMode(bool vSyncEnabled) const;
		};
	}
}



#endif // __INCLUDE_GUARD_vulkanSurface_h__