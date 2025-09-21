#include "vulkanImageUsageFlag.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
    ImageUsageFlag ImageUsageFlags::transfer_src_bit = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    ImageUsageFlag ImageUsageFlags::transfer_dst_bit = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    ImageUsageFlag ImageUsageFlags::sampled_bit = VK_IMAGE_USAGE_SAMPLED_BIT;
    ImageUsageFlag ImageUsageFlags::storage_bit = VK_IMAGE_USAGE_STORAGE_BIT;
    ImageUsageFlag ImageUsageFlags::color_attachment_bit = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    ImageUsageFlag ImageUsageFlags::depth_stencil_attachment_bit = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    ImageUsageFlag ImageUsageFlags::transient_attachment_bit = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
    ImageUsageFlag ImageUsageFlags::input_attachment_bit = VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    ImageUsageFlag ImageUsageFlags::host_transfer_bit = VK_IMAGE_USAGE_HOST_TRANSFER_BIT;
    ImageUsageFlag ImageUsageFlags::video_decode_dst_bit_khr = VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR;
    ImageUsageFlag ImageUsageFlags::video_decode_src_bit_khr = VK_IMAGE_USAGE_VIDEO_DECODE_SRC_BIT_KHR;
    ImageUsageFlag ImageUsageFlags::video_decode_dpb_bit_khr = VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR;
    ImageUsageFlag ImageUsageFlags::fragment_density_map_bit_ext = VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    ImageUsageFlag ImageUsageFlags::fragment_shading_rate_attachment_bit_khr = VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    ImageUsageFlag ImageUsageFlags::video_encode_dst_bit_khr = VK_IMAGE_USAGE_VIDEO_ENCODE_DST_BIT_KHR;
    ImageUsageFlag ImageUsageFlags::video_encode_src_bit_khr = VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR;
    ImageUsageFlag ImageUsageFlags::video_encode_dpb_bit_khr = VK_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR;
    ImageUsageFlag ImageUsageFlags::attachment_feedback_loop_bit_ext = VK_IMAGE_USAGE_ATTACHMENT_FEEDBACK_LOOP_BIT_EXT;
    ImageUsageFlag ImageUsageFlags::invocation_mask_bit_huawei = VK_IMAGE_USAGE_INVOCATION_MASK_BIT_HUAWEI;
    ImageUsageFlag ImageUsageFlags::sample_weight_bit_qcom = VK_IMAGE_USAGE_SAMPLE_WEIGHT_BIT_QCOM;
    ImageUsageFlag ImageUsageFlags::sample_block_match_bit_qcom = VK_IMAGE_USAGE_SAMPLE_BLOCK_MATCH_BIT_QCOM;
    ImageUsageFlag ImageUsageFlags::video_encode_quantization_delta_map_bit_khr = VK_IMAGE_USAGE_VIDEO_ENCODE_QUANTIZATION_DELTA_MAP_BIT_KHR;
    ImageUsageFlag ImageUsageFlags::video_encode_emphasis_map_bit_khr = VK_IMAGE_USAGE_VIDEO_ENCODE_EMPHASIS_MAP_BIT_KHR;
    ImageUsageFlag ImageUsageFlags::shading_rate_image_bit_nv = VK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV;
    ImageUsageFlag ImageUsageFlags::host_transfer_bit_ext = VK_IMAGE_USAGE_HOST_TRANSFER_BIT_EXT;
    ImageUsageFlag ImageUsageFlags::flag_bits_max_enum = VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM;

	std::string ImageUsageFlags::ToString(ImageUsageFlag imageUsageFlag)
	{
		if (imageUsageFlag == 0)
			return "none";

		std::string result;

		auto append = [&](ImageUsageFlag bit, std::string name)
		{
			if (imageUsageFlag & bit)
			{
				if (!result.empty()) result += " | ";
				result += name;
			}
		};

        append(ImageUsageFlags::transfer_src_bit, "transfer_src_bit");
        append(ImageUsageFlags::transfer_dst_bit, "transfer_dst_bit");
        append(ImageUsageFlags::sampled_bit, "sampled_bit");
        append(ImageUsageFlags::storage_bit, "storage_bit");
        append(ImageUsageFlags::color_attachment_bit, "color_attachment_bit");
        append(ImageUsageFlags::depth_stencil_attachment_bit, "depth_stencil_attachment_bit");
        append(ImageUsageFlags::transient_attachment_bit, "transient_attachment_bit");
        append(ImageUsageFlags::input_attachment_bit, "input_attachment_bit");
        append(ImageUsageFlags::host_transfer_bit, "host_transfer_bit");
        append(ImageUsageFlags::video_decode_dst_bit_khr, "video_decode_dst_bit_khr");
        append(ImageUsageFlags::video_decode_src_bit_khr, "video_decode_src_bit_khr");
        append(ImageUsageFlags::video_decode_dpb_bit_khr, "video_decode_dpb_bit_khr");
        append(ImageUsageFlags::fragment_density_map_bit_ext, "fragment_density_map_bit_ext");
        append(ImageUsageFlags::fragment_shading_rate_attachment_bit_khr, "fragment_shading_rate_attachment_bit_khr");
        append(ImageUsageFlags::video_encode_dst_bit_khr, "video_encode_dst_bit_khr");
        append(ImageUsageFlags::video_encode_src_bit_khr, "video_encode_src_bit_khr");
        append(ImageUsageFlags::video_encode_dpb_bit_khr, "video_encode_dpb_bit_khr");
        append(ImageUsageFlags::attachment_feedback_loop_bit_ext, "attachment_feedback_loop_bit_ext");
        append(ImageUsageFlags::invocation_mask_bit_huawei, "invocation_mask_bit_huawei");
        append(ImageUsageFlags::sample_weight_bit_qcom, "sample_weight_bit_qcom");
        append(ImageUsageFlags::sample_block_match_bit_qcom, "sample_block_match_bit_qcom");
        append(ImageUsageFlags::video_encode_quantization_delta_map_bit_khr, "video_encode_quantization_delta_map_bit_khr");
        append(ImageUsageFlags::video_encode_emphasis_map_bit_khr, "video_encode_emphasis_map_bit_khr");
        append(ImageUsageFlags::shading_rate_image_bit_nv, "shading_rate_image_bit_nv");
        append(ImageUsageFlags::host_transfer_bit_ext, "host_transfer_bit_ext");

		return result.empty() ? "unknown" : result;
	}
}