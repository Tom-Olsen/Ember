#include "vulkanVertexInputBindingDescriptionToString.h"
#include "vulkanVertexInputRateToString.h"
#include <sstream>
#include <vulkan/vulkan.h>



namespace emberVulkanUtility
{
	std::string ToString(VkVertexInputBindingDescription vkVertexInputBindingDescription)
	{
		std::ostringstream ss;
		ss << "binding:" << vkVertexInputBindingDescription.binding << ", ";
		ss << "stride:" << vkVertexInputBindingDescription.stride << ", ";
		ss << "inputRate:" << ToString(vkVertexInputBindingDescription.inputRate);
		return ss.str();
	}
}