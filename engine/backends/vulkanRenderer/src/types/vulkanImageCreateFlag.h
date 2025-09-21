#pragma once
#include <cstdint>
#include <string>



namespace vulkanRendererBackend
{
    using ImageCreateFlag = uint32_t;
    struct ImageCreateFlags
    {
        static ImageCreateFlag sparse_binding_bit;
        static ImageCreateFlag sparse_residency_bit;
        static ImageCreateFlag sparse_aliased_bit;
        static ImageCreateFlag mutable_format_bit;
        static ImageCreateFlag cube_compatible_bit;
        static ImageCreateFlag alias_bit;
        static ImageCreateFlag split_instance_bind_regions_bit;
        static ImageCreateFlag array_2d_compatible_bit;
        static ImageCreateFlag block_texel_view_compatible_bit;
        static ImageCreateFlag extended_usage_bit;
        static ImageCreateFlag protected_bit;
        static ImageCreateFlag disjoint_bit;
        static ImageCreateFlag corner_sampled_bit_nv;
        static ImageCreateFlag sample_locations_compatible_depth_bit_ext;
        static ImageCreateFlag subsampled_bit_ext;
        static ImageCreateFlag descriptor_buffer_capture_replay_bit_ext;
        static ImageCreateFlag multisampled_render_to_single_sampled_bit_ext;
        static ImageCreateFlag view_2d_compatible_bit_ext;
        static ImageCreateFlag fragment_density_map_offset_bit_qcom;
        static ImageCreateFlag video_profile_independent_bit_khr;
        static ImageCreateFlag flag_bits_max_enum;
        static std::string ToString(ImageCreateFlag imageCreateFlag);
    };
}