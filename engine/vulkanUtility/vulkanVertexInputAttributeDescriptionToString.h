#pragma once
#include "vulkanFormatToString.h"
#include <sstream>
#include <vulkan/vulkan.h>



namespace emberVulkanUtility
{
	inline std::string ToString(VkVertexInputAttributeDescription vkVertexInputAttributeDescription)
	{
		std::ostringstream ss;
		ss << "location:" << vkVertexInputAttributeDescription.location << ", ";
		ss << "binding:" << vkVertexInputAttributeDescription.binding << ", ";
		ss << "format:" << emberVulkanUtility::ToString(vkVertexInputAttributeDescription.format) << ", ";
		ss << "offset:" << vkVertexInputAttributeDescription.offset;
		return ss.str();
	}
}