#ifndef __INCLUDE_GUARD_vulkanContext_h__
#define __INCLUDE_GUARD_vulkanContext_h__
#include <vulkan/vulkan.h>



struct VulkanQueue
{
	VkQueue queue;
	uint32_t familyIndex;
};



class VulkanContext
{
public:
	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkPhysicalDeviceProperties physicalDeviceProperties;
	VkDevice device;
	VulkanQueue graphicsQueue;

	// Constructor:
	VulkanContext(uint32_t instanceExtensionCount, const char* const* instanceExtensions, uint32_t deviceExtensionCount, const char** deviceExtensions);

	// Destructor:
	~VulkanContext();

private:
	void CreateInstance(uint32_t instanceExtensionCount, const char* const* instanceExtensions);
	void SelectPhysicalDevice();
	uint32_t FindFirstGraphicsQueueFamilyIndex();
	void CreateDevice(uint32_t deviceExtensionCount, const char** deviceExtensions);
	bool IsDeviceSuitable(VkPhysicalDevice device);
};
#endif // __INCLUDE_GUARD_vulkanContext_h__