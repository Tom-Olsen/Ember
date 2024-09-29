#ifndef __INCLUDE_GUARD_vulkanInstance_h__
#define __INCLUDE_GUARD_vulkanInstance_h__
#include <vulkan/vulkan.h>
#include <vector>
#include "macros.h"



/// <summary>
/// VulkanInstance class owns the Vulkan instance and handles validation layers.
/// </summary>
class VulkanInstance
{
public: // Members:
	VkInstance instance;
	#if defined(VALIDATION_LAYERS_ACTIVE)
	VkDebugUtilsMessengerEXT debugMessenger;
	#endif

public: // Methods:
	VulkanInstance(std::vector<const char*> instanceExtensions);
	~VulkanInstance();

private: // Methods:
	std::vector<const char*> AvailableInstanceExtensions();
};



#endif // __INCLUDE_GUARD_vulkanInstance_h__