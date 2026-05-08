#include "vulkanImageSubresourceLayersToString.h"
#include "vulkanImageAspectFlagsToString.h"
#include <sstream>



namespace emberVulkanUtility
{
	std::string ToString(VkImageSubresourceLayers vkImageSubresourceLayers)
	{
		std::ostringstream ss;
		ss << "aspectMask: " << ToString_VkImageAspectFlags(vkImageSubresourceLayers.aspectMask)
		   << ", mipLevel: " << vkImageSubresourceLayers.mipLevel << ", baseArrayLayer: " << vkImageSubresourceLayers.baseArrayLayer
		   << ", layerCount: " << vkImageSubresourceLayers.layerCount;
		return ss.str();
	}
}