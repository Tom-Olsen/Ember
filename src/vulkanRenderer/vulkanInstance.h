#ifndef __INCLUDE_GUARD_vulkanInstance_h__
#define __INCLUDE_GUARD_vulkanInstance_h__
#include <vulkan/vulkan.h>
#include <vector>



/// <summary>
/// VulkanInstance class owns the Vulkan instance and handles validation layers.
/// </summary>
class VulkanInstance
{
public: // Members:
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;

public: // Methods:
	VulkanInstance(std::vector<const char*> instanceExtensions);
	~VulkanInstance();

private: // Methods:
	std::vector<const char*> AvailableInstanceExtensions();
};



#endif // __INCLUDE_GUARD_vulkanInstance_h__