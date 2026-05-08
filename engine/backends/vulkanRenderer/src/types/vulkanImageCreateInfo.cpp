#include "vulkanImageCreateInfo.h"
#include "vulkanImageLayoutToString.h"
#include "vulkanImageTilingToString.h"
#include "vulkanImageTypeToString.h"
#include "vulkanSharingModeToString.h"
#include <sstream>



namespace vulkanRendererBackend
{
	std::string ImageCreateInfo::ToString() const
	{
		std::ostringstream ss;
		ss << "pNext: " << pNext << ", flags: " << ImageCreateFlags::ToString(flags)
		   << ", imageType: " << emberVulkanUtility::ToString(imageType) << ", format: " << Formats::ToString(format)
		   << ", extent: " << extent.ToString() << ", mipLevels: " << mipLevels << ", arrayLayers: " << arrayLayers
		   << ", sampleCountFlags: " << SampleCountFlags::ToString(sampleCountFlags) << ", tiling: " << emberVulkanUtility::ToString(tiling)
		   << ", usages: " << ImageUsageFlags::ToString(usages) << ", sharingMode: " << emberVulkanUtility::ToString(sharingMode)
		   << ", queueFamilyIndexCount: " << queueFamilyIndexCount;
		for (int i = 0; i < queueFamilyIndexCount; i++)
			ss << pQueueFamilyIndices[i] << " ";
		ss << ", initialLayout: " << emberVulkanUtility::ToString(initialLayout);
		return ss.str();
	}
}