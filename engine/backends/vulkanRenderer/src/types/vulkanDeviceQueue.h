#pragma once
#include <string>



// Forward declarations:
typedef struct VkQueue_T* VkQueue;



namespace vulkanRendererBackend
{
	struct DeviceQueue
	{
		VkQueue queue = nullptr;
		uint32_t familyIndex = (uint32_t)-1;
		std::string ToString();
	};
}