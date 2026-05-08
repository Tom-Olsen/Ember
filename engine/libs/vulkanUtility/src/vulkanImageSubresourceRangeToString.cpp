#include "vulkanImageSubresourceRangeToString.h"
#include "vulkanImageAspectFlagsToString.h"
#include <sstream>



namespace emberVulkanUtility
{
	std::string ToString(VkImageSubresourceRange vkImageSubresourceRange)
	{
		std::ostringstream ss;
		ss << "aspectMask: " << ToString_VkImageAspectFlags(vkImageSubresourceRange.aspectMask)
		   << ", baseMipLevel: " << vkImageSubresourceRange.baseMipLevel << ", levelCount: " << vkImageSubresourceRange.levelCount
		   << ", baseArrayLayer: " << vkImageSubresourceRange.baseArrayLayer << ", layerCount: " << vkImageSubresourceRange.layerCount;
		return ss.str();
	}
}