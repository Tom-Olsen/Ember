#include "vulkanBufferCreateInfo.h"
#include <sstream>



namespace vulkanRendererBackend
{
	std::string BufferCreateInfo::ToString() const
	{
		std::ostringstream ss;
		ss << "pNext: " << pNext << ", flags: " << BufferCreateFlags::ToString(flags) << ", size: " << size
		   << ", usages: " << BufferUsageFlags::ToString(usages) << ", sharingMode: " << SharingModes::ToString(sharingMode)
		   << ", queueFamilyIndexCount: " << queueFamilyIndexCount << ", pQueueFamilyIndices: ";
		for (int i = 0; i < queueFamilyIndexCount; i++)
			ss << pQueueFamilyIndices[i] << " ";
		return ss.str();
	}
}