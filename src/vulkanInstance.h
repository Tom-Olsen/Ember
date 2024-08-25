#ifndef __INCLUDE_GUARD_vulkanInstance_h__
#define __INCLUDE_GUARD_vulkanInstance_h__
#include <vulkan/vulkan.h>
#include <vector>



class VulkanInstance
{
public: // Members:
	VkInstance instance;
	#ifndef NDEBUG // debug mode
	VkDebugUtilsMessengerEXT debugMessenger;
	#endif

public: // Methods:
	VulkanInstance(std::vector<const char*> instanceExtensions);
	~VulkanInstance();

private: // Methods:
	std::vector<const char*> AvailableInstanceExtensions();
};



#endif // __INCLUDE_GUARD_vulkanInstance_h__