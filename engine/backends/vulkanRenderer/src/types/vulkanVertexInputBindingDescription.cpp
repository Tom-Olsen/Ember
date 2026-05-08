#include "vulkanVertexInputBindingDescription.h"
#include "vulkanVertexInputRateToString.h"
#include <sstream>



namespace vulkanRendererBackend
{
	std::string VertexInputBindingDescription::ToString()
	{
		std::ostringstream ss;
		ss << "binding: " << binding << ", stride: " << stride << ", inputRate: " << emberVulkanUtility::ToString(inputRate);
		return ss.str();
	}
}