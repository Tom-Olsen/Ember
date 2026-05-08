#include "vulkanSharingModeToString.h"



namespace emberVulkanUtility
{
	std::string ToString(VkSharingMode vkSharingMode)
	{
		switch (vkSharingMode)
		{
			case VK_SHARING_MODE_EXCLUSIVE:
				return "VK_SHARING_MODE_EXCLUSIVE";
			case VK_SHARING_MODE_CONCURRENT:
				return "VK_SHARING_MODE_CONCURRENT";
			case VK_SHARING_MODE_MAX_ENUM:
				return "VK_SHARING_MODE_MAX_ENUM";
			default:
				return "unknown";
		}
	}
}