#include "vulkanImageCreateFlag.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
    ImageCreateFlag ImageCreateFlags::sparse_binding_bit = VK_IMAGE_CREATE_SPARSE_BINDING_BIT;
    ImageCreateFlag ImageCreateFlags::sparse_residency_bit = VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT;
    ImageCreateFlag ImageCreateFlags::sparse_aliased_bit = VK_IMAGE_CREATE_SPARSE_ALIASED_BIT;
    ImageCreateFlag ImageCreateFlags::mutable_format_bit = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
    ImageCreateFlag ImageCreateFlags::cube_compatible_bit = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    ImageCreateFlag ImageCreateFlags::alias_bit = VK_IMAGE_CREATE_ALIAS_BIT;
    ImageCreateFlag ImageCreateFlags::split_instance_bind_regions_bit = VK_IMAGE_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT;
    ImageCreateFlag ImageCreateFlags::array_2d_compatible_bit = VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT;
    ImageCreateFlag ImageCreateFlags::block_texel_view_compatible_bit = VK_IMAGE_CREATE_BLOCK_TEXEL_VIEW_COMPATIBLE_BIT;
    ImageCreateFlag ImageCreateFlags::extended_usage_bit = VK_IMAGE_CREATE_EXTENDED_USAGE_BIT;
    ImageCreateFlag ImageCreateFlags::protected_bit = VK_IMAGE_CREATE_PROTECTED_BIT;
    ImageCreateFlag ImageCreateFlags::disjoint_bit = VK_IMAGE_CREATE_DISJOINT_BIT;
    ImageCreateFlag ImageCreateFlags::corner_sampled_bit_nv = VK_IMAGE_CREATE_CORNER_SAMPLED_BIT_NV;
    ImageCreateFlag ImageCreateFlags::sample_locations_compatible_depth_bit_ext = VK_IMAGE_CREATE_SAMPLE_LOCATIONS_COMPATIBLE_DEPTH_BIT_EXT;
    ImageCreateFlag ImageCreateFlags::subsampled_bit_ext = VK_IMAGE_CREATE_SUBSAMPLED_BIT_EXT;
    ImageCreateFlag ImageCreateFlags::descriptor_buffer_capture_replay_bit_ext = VK_IMAGE_CREATE_DESCRIPTOR_BUFFER_CAPTURE_REPLAY_BIT_EXT;
    ImageCreateFlag ImageCreateFlags::multisampled_render_to_single_sampled_bit_ext = VK_IMAGE_CREATE_MULTISAMPLED_RENDER_TO_SINGLE_SAMPLED_BIT_EXT;
    ImageCreateFlag ImageCreateFlags::view_2d_compatible_bit_ext = VK_IMAGE_CREATE_2D_VIEW_COMPATIBLE_BIT_EXT;
    ImageCreateFlag ImageCreateFlags::fragment_density_map_offset_bit_qcom = VK_IMAGE_CREATE_FRAGMENT_DENSITY_MAP_OFFSET_BIT_QCOM;
    ImageCreateFlag ImageCreateFlags::video_profile_independent_bit_khr = VK_IMAGE_CREATE_VIDEO_PROFILE_INDEPENDENT_BIT_KHR;
	ImageCreateFlag ImageCreateFlags::flag_bits_max_enum = VK_IMAGE_CREATE_FLAG_BITS_MAX_ENUM;

	std::string ImageCreateFlags::ToString(ImageCreateFlag imageCreateFlag)
	{
		if (imageCreateFlag == 0)
			return "none";

		std::string result;

		auto append = [&](ImageCreateFlag bit, std::string name)
		{
			if (imageCreateFlag & bit)
			{
				if (!result.empty()) result += " | ";
				result += name;
			}
		};

		append(ImageCreateFlags::sparse_binding_bit, "sparse_binding_bit");
		append(ImageCreateFlags::sparse_residency_bit, "sparse_residency_bit");
		append(ImageCreateFlags::sparse_aliased_bit, "sparse_aliased_bit");
		append(ImageCreateFlags::mutable_format_bit, "mutable_format_bit");
		append(ImageCreateFlags::cube_compatible_bit, "cube_compatible_bit");
		append(ImageCreateFlags::alias_bit, "alias_bit");
		append(ImageCreateFlags::split_instance_bind_regions_bit, "split_instance_bind_regions_bit");
		append(ImageCreateFlags::array_2d_compatible_bit, "array_2d_compatible_bit");
		append(ImageCreateFlags::block_texel_view_compatible_bit, "block_texel_view_compatible_bit");
		append(ImageCreateFlags::extended_usage_bit, "extended_usage_bit");
		append(ImageCreateFlags::protected_bit, "protected_bit");
		append(ImageCreateFlags::disjoint_bit, "disjoint_bit");
		append(ImageCreateFlags::corner_sampled_bit_nv, "corner_sampled_bit_nv");
		append(ImageCreateFlags::sample_locations_compatible_depth_bit_ext, "sample_locations_compatible_depth_bit_ext");
		append(ImageCreateFlags::subsampled_bit_ext, "subsampled_bit_ext");
		append(ImageCreateFlags::descriptor_buffer_capture_replay_bit_ext, "descriptor_buffer_capture_replay_bit_ext");
		append(ImageCreateFlags::multisampled_render_to_single_sampled_bit_ext, "multisampled_render_to_single_sampled_bit_ext");
		append(ImageCreateFlags::view_2d_compatible_bit_ext, "view_2d_compatible_bit_ext");
		append(ImageCreateFlags::fragment_density_map_offset_bit_qcom, "fragment_density_map_offset_bit_qcom");
		append(ImageCreateFlags::video_profile_independent_bit_khr, "video_profile_independent_bit_khr");

		return result.empty() ? "unknown" : result;
	}
}