#ifndef __INCLUDE_GUARD_vulkanDeviceQueue_h__
#define __INCLUDE_GUARD_vulkanDeviceQueue_h__
#include <vulkan/vulkan.h>
#include <string>



namespace vulkanRendererBackend
{
	struct DeviceQueue
	{
		VkQueue queue = VK_NULL_HANDLE;
		uint32_t familyIndex = (uint32_t) - 1;
		std::string ToString() const;
	};
}



#endif // __INCLUDE_GUARD_vulkanDeviceQueue_h__