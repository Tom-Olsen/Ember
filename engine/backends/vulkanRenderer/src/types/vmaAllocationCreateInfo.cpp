#include "vmaAllocationCreateInfo.h"
#include "vulkanMemoryPropertyFlagsToString.h"
#include <sstream>



namespace vulkanRendererBackend
{
	std::string AllocationCreateInfo::ToString() const
	{
		std::ostringstream ss;
		ss << "flags: " << AllocationCreateFlags::ToString(flags) << ", usages: " << MemoryUsages::ToString(usages)
		   << ", requiredFlags: " << emberVulkanUtility::ToString_VkMemoryPropertyFlags(requiredFlags) << ", preferredFlags: " << emberVulkanUtility::ToString_VkMemoryPropertyFlags(preferredFlags)
		   << ", memoryTypeBits: " << memoryTypeBits << ", pool: " << pool << ", pUserData" << pUserData << ", priority" << priority;
		return ss.str();
	}
}