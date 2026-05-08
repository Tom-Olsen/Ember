#include "vulkanSampleCountFlagBitsToString.h"



namespace emberVulkanUtility
{
	std::string ToString(VkSampleCountFlagBits vkSampleCountFlagBits)
	{
		switch (vkSampleCountFlagBits)
		{
			case VK_SAMPLE_COUNT_1_BIT:
				return "VK_SAMPLE_COUNT_1_BIT";
			case VK_SAMPLE_COUNT_2_BIT:
				return "VK_SAMPLE_COUNT_2_BIT";
			case VK_SAMPLE_COUNT_4_BIT:
				return "VK_SAMPLE_COUNT_4_BIT";
			case VK_SAMPLE_COUNT_8_BIT:
				return "VK_SAMPLE_COUNT_8_BIT";
			case VK_SAMPLE_COUNT_16_BIT:
				return "VK_SAMPLE_COUNT_16_BIT";
			case VK_SAMPLE_COUNT_32_BIT:
				return "VK_SAMPLE_COUNT_32_BIT";
			case VK_SAMPLE_COUNT_64_BIT:
				return "VK_SAMPLE_COUNT_64_BIT";
			case VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM:
				return "VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM";
			default:
				return "unknown";
		}
	}
}