#include "vmaAllocationCreateInfoToString.h"
#include "vmaAllocationCreateFlagsToString.h"
#include "vmaMemoryUsageToString.h"
#include "vulkanMemoryPropertyFlagsToString.h"
#include <sstream>



namespace vulkanRendererBackend
{
	std::string ToString_VmaAllocationCreateInfo(const VmaAllocationCreateInfo& vmaAllocationCreateInfo)
	{
		std::ostringstream ss;
		ss << "flags: " << ToString_VmaAllocationCreateFlags(vmaAllocationCreateInfo.flags)
		   << ", usage: " << ToString_VmaMemoryUsage(vmaAllocationCreateInfo.usage)
		   << ", requiredFlags: " << emberVulkanUtility::ToString_VkMemoryPropertyFlags(vmaAllocationCreateInfo.requiredFlags)
		   << ", preferredFlags: " << emberVulkanUtility::ToString_VkMemoryPropertyFlags(vmaAllocationCreateInfo.preferredFlags)
		   << ", memoryTypeBits: " << vmaAllocationCreateInfo.memoryTypeBits
		   << ", pool: " << vmaAllocationCreateInfo.pool
		   << ", pUserData: " << vmaAllocationCreateInfo.pUserData
		   << ", priority: " << vmaAllocationCreateInfo.priority;
		return ss.str();
	}
}