#include "vulkanIndexTypeToString.h"



namespace emberVulkanUtility
{
	std::string ToString(VkIndexType vkIndexType)
	{
		switch (vkIndexType)
		{
			case VK_INDEX_TYPE_UINT16:
				return "VK_INDEX_TYPE_UINT16";
			case VK_INDEX_TYPE_UINT32:
				return "VK_INDEX_TYPE_UINT32";
			case VK_INDEX_TYPE_UINT8:
				return "VK_INDEX_TYPE_UINT8";
			case VK_INDEX_TYPE_NONE_KHR:
				return "VK_INDEX_TYPE_NONE_KHR";
			case VK_INDEX_TYPE_MAX_ENUM:
				return "VK_INDEX_TYPE_MAX_ENUM";
			default:
				return "unknown";
		}
	}
}