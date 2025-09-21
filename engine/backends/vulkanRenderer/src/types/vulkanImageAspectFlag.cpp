#include "vulkanImageAspectFlag.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
    ImageAspectFlag ImageAspectFlags::none = VK_IMAGE_ASPECT_NONE;
    ImageAspectFlag ImageAspectFlags::color_bit = VK_IMAGE_ASPECT_COLOR_BIT;
    ImageAspectFlag ImageAspectFlags::depth_bit = VK_IMAGE_ASPECT_DEPTH_BIT;
    ImageAspectFlag ImageAspectFlags::stencil_bit = VK_IMAGE_ASPECT_STENCIL_BIT;
    ImageAspectFlag ImageAspectFlags::metadata_bit = VK_IMAGE_ASPECT_METADATA_BIT;
    ImageAspectFlag ImageAspectFlags::plane_0_bit = VK_IMAGE_ASPECT_PLANE_0_BIT;
    ImageAspectFlag ImageAspectFlags::plane_1_bit = VK_IMAGE_ASPECT_PLANE_1_BIT;
    ImageAspectFlag ImageAspectFlags::plane_2_bit = VK_IMAGE_ASPECT_PLANE_2_BIT;
    ImageAspectFlag ImageAspectFlags::memory_plane_0_bit_ext = VK_IMAGE_ASPECT_MEMORY_PLANE_0_BIT_EXT;
    ImageAspectFlag ImageAspectFlags::memory_plane_1_bit_ext = VK_IMAGE_ASPECT_MEMORY_PLANE_1_BIT_EXT;
    ImageAspectFlag ImageAspectFlags::memory_plane_2_bit_ext = VK_IMAGE_ASPECT_MEMORY_PLANE_2_BIT_EXT;
    ImageAspectFlag ImageAspectFlags::memory_plane_3_bit_ext = VK_IMAGE_ASPECT_MEMORY_PLANE_3_BIT_EXT;
	ImageAspectFlag ImageAspectFlags::flag_bits_max_enum = VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM;

	std::string ImageAspectFlags::ToString(ImageAspectFlag imageAscpectFlag)
	{
		if (imageAscpectFlag == 0)
			return "none";

		std::string result;

		auto append = [&](ImageAspectFlag bit, std::string name)
		{
			if (imageAscpectFlag & bit)
			{
				if (!result.empty()) result += " | ";
				result += name;
			}
		};

		append(ImageAspectFlags::none, "none");
		append(ImageAspectFlags::color_bit, "color_bit");
		append(ImageAspectFlags::depth_bit, "depth_bit");
		append(ImageAspectFlags::stencil_bit, "stencil_bit");
		append(ImageAspectFlags::metadata_bit, "metadata_bit");
		append(ImageAspectFlags::plane_0_bit, "plane_0_bit");
		append(ImageAspectFlags::plane_1_bit, "plane_1_bit");
		append(ImageAspectFlags::plane_2_bit, "plane_2_bit");
		append(ImageAspectFlags::memory_plane_0_bit_ext, "memory_plane_0_bit_ext");
		append(ImageAspectFlags::memory_plane_1_bit_ext, "memory_plane_1_bit_ext");
		append(ImageAspectFlags::memory_plane_2_bit_ext, "memory_plane_2_bit_ext");
		append(ImageAspectFlags::memory_plane_3_bit_ext, "memory_plane_3_bit_ext");

		return result.empty() ? "unknown" : result;
	}
}