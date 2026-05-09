#include "vulkanBufferCreateInfo.h"
#include "vulkanBufferCreateFlagsToString.h"
#include "vulkanBufferUsageFlagsToString.h"
#include "vulkanSharingModeToString.h"
#include <sstream>



namespace vulkanRendererBackend
{
	std::string BufferCreateInfo::ToString() const
	{
		std::ostringstream ss;
		ss << "pNext: " << pNext << ", flags: " << emberVulkanUtility::ToString_VkBufferCreateFlags(flags) << ", size: " << size
		   << ", usages: " << emberVulkanUtility::ToString_VkBufferUsageFlags(usages) << ", sharingMode: " << emberVulkanUtility::ToString(sharingMode)
		   << ", queueFamilyIndexCount: " << queueFamilyIndexCount << ", pQueueFamilyIndices: ";
		for (int i = 0; i < queueFamilyIndexCount; i++)
			ss << pQueueFamilyIndices[i] << " ";
		return ss.str();
	}
}