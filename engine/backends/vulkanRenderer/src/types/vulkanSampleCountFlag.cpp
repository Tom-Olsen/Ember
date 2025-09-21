#include "vulkanSampleCountFlag.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
    SampleCountFlag SampleCountFlags::sample_count_1_bit = VK_SAMPLE_COUNT_1_BIT;
    SampleCountFlag SampleCountFlags::sample_count_2_bit = VK_SAMPLE_COUNT_2_BIT;
    SampleCountFlag SampleCountFlags::sample_count_4_bit = VK_SAMPLE_COUNT_4_BIT;
    SampleCountFlag SampleCountFlags::sample_count_8_bit = VK_SAMPLE_COUNT_8_BIT;
    SampleCountFlag SampleCountFlags::sample_count_16_bit = VK_SAMPLE_COUNT_16_BIT;
    SampleCountFlag SampleCountFlags::sample_count_32_bit = VK_SAMPLE_COUNT_32_BIT;
    SampleCountFlag SampleCountFlags::sample_count_64_bit = VK_SAMPLE_COUNT_64_BIT;
	SampleCountFlag SampleCountFlags::sample_count_flag_bits_max_enum = VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;

	std::string SampleCountFlags::ToString(SampleCountFlag sampleCountFlag)
	{
		if (sampleCountFlag == 0)
			return "none";

		std::string result;

		auto append = [&](SampleCountFlag bit, std::string name)
		{
			if (sampleCountFlag & bit)
			{
				if (!result.empty()) result += " | ";
				result += name;
			}
		};

		append(SampleCountFlags::sample_count_1_bit, "sample_count_1_bit");
		append(SampleCountFlags::sample_count_2_bit, "sample_count_2_bit");
		append(SampleCountFlags::sample_count_4_bit, "sample_count_4_bit");
		append(SampleCountFlags::sample_count_8_bit, "sample_count_8_bit");
		append(SampleCountFlags::sample_count_16_bit, "sample_count_16_bit");
		append(SampleCountFlags::sample_count_32_bit, "sample_count_32_bit");
		append(SampleCountFlags::sample_count_64_bit, "sample_count_64_bit");

		return result.empty() ? "unknown" : result;
	}
}