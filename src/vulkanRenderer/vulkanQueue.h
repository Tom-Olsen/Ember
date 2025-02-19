#ifndef __INCLUDE_GUARD_vulkanQueue_h__
#define __INCLUDE_GUARD_vulkanQueue_h__
#include <vulkan/vulkan.h>



namespace emberEngine
{
	struct VulkanQueue
	{
		VkQueue queue;
		uint32_t familyIndex;
	};
}



#endif // __INCLUDE_GUARD_vulkanQueue_h__