#pragma once
#include <sstream>
#include <vulkan/vulkan.h>



namespace emberVulkanUtility
{
    inline std::string ToString(VkImageViewType vkImageViewType)
    {
		std::ostringstream ss;
		switch (vkImageViewType)
		{
			case VK_IMAGE_VIEW_TYPE_1D:			ss << "VK_IMAGE_VIEW_TYPE_1D"; break;
			case VK_IMAGE_VIEW_TYPE_2D:			ss << "VK_IMAGE_VIEW_TYPE_2D"; break;
			case VK_IMAGE_VIEW_TYPE_3D:			ss << "VK_IMAGE_VIEW_TYPE_3D"; break;
			case VK_IMAGE_VIEW_TYPE_CUBE:		ss << "VK_IMAGE_VIEW_TYPE_CUBE"; break;
			case VK_IMAGE_VIEW_TYPE_1D_ARRAY:	ss << "VK_IMAGE_VIEW_TYPE_1D_ARRAY"; break;
			case VK_IMAGE_VIEW_TYPE_2D_ARRAY:	ss << "VK_IMAGE_VIEW_TYPE_2D_ARRAY"; break;
			case VK_IMAGE_VIEW_TYPE_CUBE_ARRAY:	ss << "VK_IMAGE_VIEW_TYPE_CUBE_ARRAY"; break;
			case VK_IMAGE_VIEW_TYPE_MAX_ENUM:	ss << "VK_IMAGE_VIEW_TYPE_MAX_ENUM"; break;
			default: ss << "unknown";
		}
		return ss.str();
    }
}