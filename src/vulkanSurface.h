#ifndef __INCLUDE_GUARD_vulkanSurface_h__
#define __INCLUDE_GUARD_vulkanSurface_h__
#include <SDL3/SDL.h>
#include "sdlWindow.h"
#include "vulkanInstance.h"
#include "vulkanPhysicalDevice.h"



class VulkanSurface
{
public: // Members:
	VkSurfaceKHR surface;
	VkSurfaceFormatKHR surfaceFormat;
	VkPresentModeKHR presentMode;

private: // Members:
	VulkanInstance* instance;
	VulkanPhysicalDevice* physicalDevice;
	SdlWindow* window;

	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	std::vector<VkSurfaceFormatKHR> availableSurfaceFormats;
	std::vector<VkPresentModeKHR> availablePresentModes;

public: // Methods:
	VulkanSurface(VulkanInstance* instance, VulkanPhysicalDevice* physicalDevice, SdlWindow* window);
	~VulkanSurface();
	VkExtent2D CurrentExtent();
	VkExtent2D MinImageExtent();
	VkExtent2D MaxImageExtent();
	uint32_t MinImageCount();
	uint32_t MaxImageCount();

private: // Methods:
	VkSurfaceFormatKHR PickSurfaceFormat();
	VkPresentModeKHR PickPresentMode();
};



#endif // __INCLUDE_GUARD_vulkanSurface_h__