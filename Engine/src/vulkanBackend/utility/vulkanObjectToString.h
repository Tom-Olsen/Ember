#ifndef __INCLUDE_GUARD_vulkanObjectToString_h__
#define __INCLUDE_GUARD_vulkanObjectToString_h__
#include <string>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	namespace vulkanBackend
	{
		namespace objectToString
		{
			std::string VkAccessFlagBits2ToString(VkAccessFlagBits2 vkAccessFlagBits2);
			std::string VkDescriptorTypeToString(VkDescriptorType vkDescriptorType);
			std::string VkFormatToString(VkFormat vkFormat);
			std::string VkImageLayoutToString(VkImageLayout vkImageView);
			std::string VkImageViewTypeToString(VkImageViewType vkImageViewType);
			std::string VkShaderStageFlagsToString(VkShaderStageFlags vkShaderStageFlags);
			std::string VkVertexInputAttributeDescriptionToString(VkVertexInputAttributeDescription vkVertexInputAttributeDescription);
			std::string VkVertexInputRateToString(VkVertexInputRate vkVertexInputRate);
		}
	}
}



#endif // __INCLUDE_GUARD_vulkanObjectToString_h__