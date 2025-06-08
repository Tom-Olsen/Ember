#ifndef __INCLUDE_GUARD_vulkanDeviceQueue_h__
#define __INCLUDE_GUARD_vulkanDeviceQueue_h__
#include <vulkan/vulkan.h>
#include <string>



namespace emberEngine
{
	namespace vulkanBackend
	{
		struct DeviceQueue
		{
			VkQueue queue;
			uint32_t familyIndex;
			std::string ToString() const;
		};
	}
}



#endif // __INCLUDE_GUARD_vulkanDeviceQueue_h__