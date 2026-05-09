#include "vulkanImageCreateInfo.h"
#include "vulkanImageCreateFlagsToString.h"
#include "vulkanImageLayoutToString.h"
#include "vulkanImageTilingToString.h"
#include "vulkanImageTypeToString.h"
#include "vulkanImageUsageFlagsToString.h"
#include "vulkanSampleCountFlagBitsToString.h"
#include "vulkanSharingModeToString.h"
#include <sstream>



namespace vulkanRendererBackend
{
	std::string ImageCreateInfo::ToString() const
	{
		std::ostringstream ss;
		ss << "pNext: " << pNext << ", flags: " << emberVulkanUtility::ToString_VkImageCreateFlags(flags)
		   << ", imageType: " << emberVulkanUtility::ToString(imageType) << ", format: " << Formats::ToString(format)
		   << ", extent: " << extent.ToString() << ", mipLevels: " << mipLevels << ", arrayLayers: " << arrayLayers
		   << ", sampleCountFlags: " << emberVulkanUtility::ToString(sampleCountFlags) << ", tiling: " << emberVulkanUtility::ToString(tiling)
		   << ", usages: " << emberVulkanUtility::ToString_VkImageUsageFlags(usages) << ", sharingMode: " << emberVulkanUtility::ToString(sharingMode)
		   << ", queueFamilyIndexCount: " << queueFamilyIndexCount;
		for (int i = 0; i < queueFamilyIndexCount; i++)
			ss << pQueueFamilyIndices[i] << " ";
		ss << ", initialLayout: " << emberVulkanUtility::ToString(initialLayout);
		return ss.str();
	}
}