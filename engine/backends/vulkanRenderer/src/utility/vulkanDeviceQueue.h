#pragma once
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