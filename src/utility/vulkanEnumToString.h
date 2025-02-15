#ifndef __INCLUDE_GUARD_vulkanEnumToString_h__
#define __INCLUDE_GUARD_vulkanEnumToString_h__
#include <string>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	namespace vulkanEnumToString
	{
		std::string VkVertexInputRateToString(VkVertexInputRate vkVertexInputRate);
		std::string VkDescriptorTypeToString(VkDescriptorType vkDescriptorType);
		std::string VkFormatToString(VkFormat vkFormat);
		std::string VkShaderStageFlagsToString(VkShaderStageFlags vkShaderStageFlags);
	}
}



#endif // __INCLUDE_GUARD_vulkanEnumToString_h__