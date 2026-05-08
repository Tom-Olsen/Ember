#include "vulkanImageTypeToString.h"



namespace emberVulkanUtility
{
	std::string ToString(VkImageType vkImageType)
	{
		switch (vkImageType)
		{
			case VK_IMAGE_TYPE_1D:
				return "VK_IMAGE_TYPE_1D";
			case VK_IMAGE_TYPE_2D:
				return "VK_IMAGE_TYPE_2D";
			case VK_IMAGE_TYPE_3D:
				return "VK_IMAGE_TYPE_3D";
			case VK_IMAGE_TYPE_MAX_ENUM:
				return "VK_IMAGE_TYPE_MAX_ENUM";
			default:
				return "unknown";
		}
	}
}