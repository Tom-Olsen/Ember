#ifndef __INCLUDE_GUARD_vulkanInstance_h__
#define __INCLUDE_GUARD_vulkanInstance_h__
#include <vulkan/vulkan.h>
#include <vector>



class VulkanInstance
{
private: // Members:
	VkInstance m_instance;
	VkDebugUtilsMessengerEXT m_debugMessenger;

public: // Methods:
	VulkanInstance(std::vector<const char*> instanceExtensions);
	~VulkanInstance();

	const VkInstance& GetVkInstance() const;
	std::vector<const char*> AvailableInstanceExtensions() const;
};



#endif // __INCLUDE_GUARD_vulkanInstance_h__