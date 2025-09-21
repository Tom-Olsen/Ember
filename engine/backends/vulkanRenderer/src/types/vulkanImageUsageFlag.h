#pragma once
#include <cstdint>
#include <string>



namespace vulkanRendererBackend
{
    using ImageUsageFlag = uint32_t;
    struct ImageUsageFlags
    {
        static ImageUsageFlag transfer_src_bit;
        static ImageUsageFlag transfer_dst_bit;
        static ImageUsageFlag sampled_bit;
        static ImageUsageFlag storage_bit;
        static ImageUsageFlag color_attachment_bit;
        static ImageUsageFlag depth_stencil_attachment_bit;
        static ImageUsageFlag transient_attachment_bit;
        static ImageUsageFlag input_attachment_bit;
        static ImageUsageFlag host_transfer_bit;
        static ImageUsageFlag video_decode_dst_bit_khr;
        static ImageUsageFlag video_decode_src_bit_khr;
        static ImageUsageFlag video_decode_dpb_bit_khr;
        static ImageUsageFlag fragment_density_map_bit_ext;
        static ImageUsageFlag fragment_shading_rate_attachment_bit_khr;
        static ImageUsageFlag video_encode_dst_bit_khr;
        static ImageUsageFlag video_encode_src_bit_khr;
        static ImageUsageFlag video_encode_dpb_bit_khr;
        static ImageUsageFlag attachment_feedback_loop_bit_ext;
        static ImageUsageFlag invocation_mask_bit_huawei;
        static ImageUsageFlag sample_weight_bit_qcom;
        static ImageUsageFlag sample_block_match_bit_qcom;
        static ImageUsageFlag video_encode_quantization_delta_map_bit_khr;
        static ImageUsageFlag video_encode_emphasis_map_bit_khr;
        static ImageUsageFlag shading_rate_image_bit_nv;
        static ImageUsageFlag host_transfer_bit_ext;
        static ImageUsageFlag flag_bits_max_enum;
        static std::string ToString(ImageUsageFlag imageUsageFlag);
    };
}