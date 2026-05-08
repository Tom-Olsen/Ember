#include "vulkanImageAspectFlagsToString.h"
#include <sstream>



namespace emberVulkanUtility
{
	std::string ToString_VkImageAspectFlags(VkImageAspectFlags vkImageAspectFlags)
	{
		if (vkImageAspectFlags == 0)
			return "VK_IMAGE_ASPECT_NONE";

		std::ostringstream ss;
		bool first = true;
		VkImageAspectFlags knownFlags = 0;
		auto append = [&](VkImageAspectFlagBits bit, const char* name)
		{
			knownFlags |= bit;
			if (vkImageAspectFlags & bit)
			{
				if (!first)
					ss << " | ";
				first = false;
				ss << name;
			}
		};

		append(VK_IMAGE_ASPECT_COLOR_BIT, "VK_IMAGE_ASPECT_COLOR_BIT");
		append(VK_IMAGE_ASPECT_DEPTH_BIT, "VK_IMAGE_ASPECT_DEPTH_BIT");
		append(VK_IMAGE_ASPECT_STENCIL_BIT, "VK_IMAGE_ASPECT_STENCIL_BIT");
		append(VK_IMAGE_ASPECT_METADATA_BIT, "VK_IMAGE_ASPECT_METADATA_BIT");
		append(VK_IMAGE_ASPECT_PLANE_0_BIT, "VK_IMAGE_ASPECT_PLANE_0_BIT");
		append(VK_IMAGE_ASPECT_PLANE_1_BIT, "VK_IMAGE_ASPECT_PLANE_1_BIT");
		append(VK_IMAGE_ASPECT_PLANE_2_BIT, "VK_IMAGE_ASPECT_PLANE_2_BIT");
		append(VK_IMAGE_ASPECT_MEMORY_PLANE_0_BIT_EXT, "VK_IMAGE_ASPECT_MEMORY_PLANE_0_BIT_EXT");
		append(VK_IMAGE_ASPECT_MEMORY_PLANE_1_BIT_EXT, "VK_IMAGE_ASPECT_MEMORY_PLANE_1_BIT_EXT");
		append(VK_IMAGE_ASPECT_MEMORY_PLANE_2_BIT_EXT, "VK_IMAGE_ASPECT_MEMORY_PLANE_2_BIT_EXT");
		append(VK_IMAGE_ASPECT_MEMORY_PLANE_3_BIT_EXT, "VK_IMAGE_ASPECT_MEMORY_PLANE_3_BIT_EXT");

		VkImageAspectFlags unknownFlags = vkImageAspectFlags & ~knownFlags;
		if (unknownFlags != 0)
		{
			if (!first)
				ss << " | ";
			ss << "UNKNOWN_IMAGE_ASPECT_BITS(0x" << std::hex << static_cast<uint32_t>(unknownFlags) << std::dec << ")";
		}

		return ss.str();
	}
}