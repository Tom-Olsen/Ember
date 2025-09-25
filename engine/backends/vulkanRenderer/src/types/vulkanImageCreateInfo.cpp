#include "vulkanImageCreateInfo.h"
#include <sstream>



namespace vulkanRendererBackend
{
	std::string ImageCreateInfo::ToString() const
	{
		std::ostringstream ss;
		ss << "pNext: " << pNext << ", flags: " << ImageCreateFlags::ToString(flags)
		   << ", imageType: " << ImageTypes::ToString(imageType) << ", format: " << Formats::ToString(format)
		   << ", extent: " << extent.ToString() << ", mipLevels: " << mipLevels << ", arrayLayers: " << arrayLayers
		   << ", sampleCountFlags: " << SampleCountFlags::ToString(sampleCountFlags) << ", tiling: " << ImageTilings::ToString(tiling)
		   << ", usages: " << ImageUsageFlags::ToString(usages) << ", sharingMode: " << SharingModes::ToString(sharingMode)
		   << ", queueFamilyIndexCount: " << queueFamilyIndexCount;
		for (int i = 0; i < queueFamilyIndexCount; i++)
			ss << pQueueFamilyIndices[i] << " ";
		ss << ", initialLayout: " << ImageLayouts::ToString(initialLayout);
		return ss.str();
	}
}