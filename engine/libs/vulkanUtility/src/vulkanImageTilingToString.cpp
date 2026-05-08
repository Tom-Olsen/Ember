#include "vulkanImageTilingToString.h"



namespace emberVulkanUtility
{
	std::string ToString(VkImageTiling vkImageTiling)
	{
		switch (vkImageTiling)
		{
			case VK_IMAGE_TILING_OPTIMAL:
				return "VK_IMAGE_TILING_OPTIMAL";
			case VK_IMAGE_TILING_LINEAR:
				return "VK_IMAGE_TILING_LINEAR";
			case VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT:
				return "VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT";
			case VK_IMAGE_TILING_MAX_ENUM:
				return "VK_IMAGE_TILING_MAX_ENUM";
			default:
				return "unknown";
		}
	}
}