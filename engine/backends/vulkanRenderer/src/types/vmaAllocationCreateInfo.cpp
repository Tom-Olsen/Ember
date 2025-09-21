#include "vmaAllocationCreateInfo.h"
#include <sstream>



namespace vulkanRendererBackend
{
	std::string AllocationCreateInfo::ToString() const
	{
		std::ostringstream ss;
		ss << "flags: " << AllocationCreateFlags::ToString(flags) << ", usages: " << MemoryUsages::ToString(usages)
		   << ", requiredFlags: " << MemoryPropertyFlags::ToString(requiredFlags) << ", preferredFlags: " << MemoryPropertyFlags::ToString(preferredFlags)
		   << ", memoryTypeBits: " << memoryTypeBits << ", pool: " << pool << ", pUserData" << pUserData << ", priority" << priority;
		return ss.str();
	}
}