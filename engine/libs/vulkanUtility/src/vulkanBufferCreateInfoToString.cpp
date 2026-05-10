#include "vulkanBufferCreateInfoToString.h"
#include "vulkanBufferCreateFlagsToString.h"
#include "vulkanBufferUsageFlagsToString.h"
#include "vulkanSharingModeToString.h"
#include <sstream>



namespace emberVulkanUtility
{
	std::string ToString(const VkBufferCreateInfo& vkBufferCreateInfo)
	{
		std::ostringstream ss;
		ss << "sType: " << vkBufferCreateInfo.sType
		   << ", pNext: " << vkBufferCreateInfo.pNext
		   << ", flags: " << ToString_VkBufferCreateFlags(vkBufferCreateInfo.flags)
		   << ", size: " << vkBufferCreateInfo.size
		   << ", usage: " << ToString_VkBufferUsageFlags(vkBufferCreateInfo.usage)
		   << ", sharingMode: " << ToString(vkBufferCreateInfo.sharingMode)
		   << ", queueFamilyIndexCount: " << vkBufferCreateInfo.queueFamilyIndexCount
		   << ", pQueueFamilyIndices: ";

		if (vkBufferCreateInfo.pQueueFamilyIndices == nullptr)
		{
			ss << "null";
			return ss.str();
		}

		for (uint32_t i = 0; i < vkBufferCreateInfo.queueFamilyIndexCount; i++)
			ss << vkBufferCreateInfo.pQueueFamilyIndices[i] << " ";
		return ss.str();
	}
}