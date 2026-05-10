#include "vulkanImageCreateInfoToString.h"
#include "vulkanFormatToString.h"
#include "vulkanImageCreateFlagsToString.h"
#include "vulkanImageLayoutToString.h"
#include "vulkanImageTilingToString.h"
#include "vulkanImageTypeToString.h"
#include "vulkanImageUsageFlagsToString.h"
#include "vulkanSampleCountFlagBitsToString.h"
#include "vulkanSharingModeToString.h"
#include <sstream>



namespace emberVulkanUtility
{
	std::string ToString(const VkImageCreateInfo& vkImageCreateInfo)
	{
		std::ostringstream ss;
		ss << "sType: " << vkImageCreateInfo.sType
		   << ", pNext: " << vkImageCreateInfo.pNext
		   << ", flags: " << ToString_VkImageCreateFlags(vkImageCreateInfo.flags)
		   << ", imageType: " << ToString(vkImageCreateInfo.imageType)
		   << ", format: " << ToString(vkImageCreateInfo.format)
		   << ", extent: (" << vkImageCreateInfo.extent.width << ", " << vkImageCreateInfo.extent.height << ", " << vkImageCreateInfo.extent.depth << ")"
		   << ", mipLevels: " << vkImageCreateInfo.mipLevels
		   << ", arrayLayers: " << vkImageCreateInfo.arrayLayers
		   << ", samples: " << ToString(vkImageCreateInfo.samples)
		   << ", tiling: " << ToString(vkImageCreateInfo.tiling)
		   << ", usage: " << ToString_VkImageUsageFlags(vkImageCreateInfo.usage)
		   << ", sharingMode: " << ToString(vkImageCreateInfo.sharingMode)
		   << ", queueFamilyIndexCount: " << vkImageCreateInfo.queueFamilyIndexCount
		   << ", pQueueFamilyIndices: ";

		if (vkImageCreateInfo.pQueueFamilyIndices == nullptr)
			ss << "null";
		else
			for (uint32_t i = 0; i < vkImageCreateInfo.queueFamilyIndexCount; i++)
				ss << vkImageCreateInfo.pQueueFamilyIndices[i] << " ";

		ss << ", initialLayout: " << ToString(vkImageCreateInfo.initialLayout);
		return ss.str();
	}
}