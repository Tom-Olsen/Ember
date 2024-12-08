#ifndef __INCLUDE_GUARD_vulkanSurface_h__
#define __INCLUDE_GUARD_vulkanSurface_h__
#include <vulkan/vulkan.h>
#include <vector>



class VulkanInstance;
class VulkanPhysicalDevice;
class SdlWindow;



class VulkanSurface
{
private: // Members:
	VkSurfaceKHR m_surface;
	VkSurfaceFormatKHR m_surfaceFormat;
	VkPresentModeKHR m_presentMode;
	VkSurfaceCapabilitiesKHR m_surfaceCapabilities;
	std::vector<VkSurfaceFormatKHR> m_availableSurfaceFormats;
	std::vector<VkPresentModeKHR> m_availablePresentModes;
	VulkanInstance* m_pInstance;
	VulkanPhysicalDevice* m_pPhysicalDevice;
	SdlWindow* m_pWindow;

public: // Methods:
	VulkanSurface(VulkanInstance* pInstance, VulkanPhysicalDevice* pPhysicalDevice, SdlWindow* pWindow);
	~VulkanSurface();

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
	VkPresentModeKHR PickPresentMode() const;
};



#endif // __INCLUDE_GUARD_vulkanSurface_h__