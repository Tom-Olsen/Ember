#include "vulkanDeviceQueue.h"
#include <sstream>



namespace vulkanRendererBackend
{
	std::string DeviceQueue::ToString()
	{
		std::stringstream ss;
		ss << "(" << familyIndex << "," << (void*)queue << ")";
		return ss.str();
	}
}