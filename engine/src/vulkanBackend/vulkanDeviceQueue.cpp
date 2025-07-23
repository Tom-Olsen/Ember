#include "vulkanDeviceQueue.h"
#include <sstream>



namespace emberEngine
{
	namespace vulkanBackend
	{
		std::string DeviceQueue::ToString() const
		{
			std::stringstream ss;
			ss << "(" << familyIndex << "," << (void*)queue << ")";
			return ss.str();
		}
	}
}