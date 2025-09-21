#pragma once
#include <cstdint>
#include <string>



namespace vulkanRendererBackend
{
    using ImageLayout = uint32_t;
    struct ImageLayouts
    {
        static ImageLayout undefined;
        static ImageLayout general;
        static ImageLayout color_attachment_optimal;
        static ImageLayout depth_stencil_attachment_optimal;
        static ImageLayout depth_stencil_read_only_optimal;
        static ImageLayout shader_read_only_optimal;
        static ImageLayout transfer_src_optimal;
        static ImageLayout transfer_dst_optimal;
        static ImageLayout preinitialized;
        static ImageLayout depth_read_only_stencil_attachment_optimal;
        static ImageLayout depth_attachment_stencil_read_only_optimal;
        static ImageLayout depth_attachment_optimal;
        static ImageLayout depth_read_only_optimal;
        static ImageLayout stencil_attachment_optimal;
        static ImageLayout stencil_read_only_optimal;
        static ImageLayout read_only_optimal;
        static ImageLayout attachment_optimal;
        static ImageLayout rendering_local_read;
        static ImageLayout present_src_khr;
        static ImageLayout video_decode_dst_khr;
        static ImageLayout video_decode_src_khr;
        static ImageLayout video_decode_dpb_khr;
        static ImageLayout shared_present_khr;
        static ImageLayout fragment_density_map_optimal_ext;
        static ImageLayout fragment_shading_rate_attachment_optimal_khr;
        static ImageLayout video_encode_dst_khr;
        static ImageLayout video_encode_src_khr;
        static ImageLayout video_encode_dpb_khr;
        static ImageLayout attachment_feedback_loop_optimal_ext;
        static ImageLayout video_encode_quantization_map_khr;
        static ImageLayout max_enum;
        static std::string ToString(ImageLayout imageLayout);
    };
}