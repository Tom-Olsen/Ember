#ifndef __INCLUDE_GUARD_vulkanContext_h__
#define __INCLUDE_GUARD_vulkanContext_h__
#include "sdlWindow.h"
#include "vulkanInstance.h"
#include "vulkanPhysicalDevice.h"
#include "vulkanSurface.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMemoryAllocator.h"



class VulkanContext
{
public: // Members:
	SdlWindow* window;
	VulkanInstance* instance;
	VulkanPhysicalDevice* physicalDevice;
	VulkanSurface* surface;
	VulkanLogicalDevice* logicalDevice;
	VulkanMemoryAllocator* allocator;
	uint32_t framesInFlight;

private: // Members:

public: // Methods:
	VulkanContext(SdlWindow* window, VulkanInstance* instance, VulkanPhysicalDevice* physicalDevice, VulkanSurface* surface, VulkanLogicalDevice* logicalDevice, VulkanMemoryAllocator* allocator, uint32_t framesInFlight);
	~VulkanContext();
	SDL_Window* Window();
	VkInstance& Instance();
	VkPhysicalDevice& PhysicalDevice();
	VkSurfaceKHR& Surface();
	VkDevice& LogicalDevice();
	VmaAllocator& Allocator();


private: // Methods:
};



#endif // __INCLUDE_GUARD_vulkanContext_h__