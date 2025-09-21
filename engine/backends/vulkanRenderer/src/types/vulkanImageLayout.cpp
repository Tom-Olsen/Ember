#include "vulkanImageLayout.h"
#include <unordered_map>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	ImageLayout ImageLayouts::undefined = VK_IMAGE_LAYOUT_UNDEFINED;
	ImageLayout ImageLayouts::general = VK_IMAGE_LAYOUT_GENERAL;
	ImageLayout ImageLayouts::color_attachment_optimal = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	ImageLayout ImageLayouts::depth_stencil_attachment_optimal = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	ImageLayout ImageLayouts::depth_stencil_read_only_optimal = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
	ImageLayout ImageLayouts::shader_read_only_optimal = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	ImageLayout ImageLayouts::transfer_src_optimal = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	ImageLayout ImageLayouts::transfer_dst_optimal = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	ImageLayout ImageLayouts::preinitialized = VK_IMAGE_LAYOUT_PREINITIALIZED;
	ImageLayout ImageLayouts::depth_read_only_stencil_attachment_optimal = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL;
	ImageLayout ImageLayouts::depth_attachment_stencil_read_only_optimal = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL;
	ImageLayout ImageLayouts::depth_attachment_optimal = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
	ImageLayout ImageLayouts::depth_read_only_optimal = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
	ImageLayout ImageLayouts::stencil_attachment_optimal = VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;
	ImageLayout ImageLayouts::stencil_read_only_optimal = VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL;
	ImageLayout ImageLayouts::read_only_optimal = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
	ImageLayout ImageLayouts::attachment_optimal = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
	ImageLayout ImageLayouts::rendering_local_read = VK_IMAGE_LAYOUT_RENDERING_LOCAL_READ;
	ImageLayout ImageLayouts::present_src_khr = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	ImageLayout ImageLayouts::video_decode_dst_khr = VK_IMAGE_LAYOUT_VIDEO_DECODE_DST_KHR;
	ImageLayout ImageLayouts::video_decode_src_khr = VK_IMAGE_LAYOUT_VIDEO_DECODE_SRC_KHR;
	ImageLayout ImageLayouts::video_decode_dpb_khr = VK_IMAGE_LAYOUT_VIDEO_DECODE_DPB_KHR;
	ImageLayout ImageLayouts::shared_present_khr = VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR;
	ImageLayout ImageLayouts::fragment_density_map_optimal_ext = VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT;
	ImageLayout ImageLayouts::fragment_shading_rate_attachment_optimal_khr = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;
	ImageLayout ImageLayouts::video_encode_dst_khr = VK_IMAGE_LAYOUT_VIDEO_ENCODE_DST_KHR;
	ImageLayout ImageLayouts::video_encode_src_khr = VK_IMAGE_LAYOUT_VIDEO_ENCODE_SRC_KHR;
	ImageLayout ImageLayouts::video_encode_dpb_khr = VK_IMAGE_LAYOUT_VIDEO_ENCODE_DPB_KHR;
	ImageLayout ImageLayouts::attachment_feedback_loop_optimal_ext = VK_IMAGE_LAYOUT_ATTACHMENT_FEEDBACK_LOOP_OPTIMAL_EXT;
	ImageLayout ImageLayouts::video_encode_quantization_map_khr = VK_IMAGE_LAYOUT_VIDEO_ENCODE_QUANTIZATION_MAP_KHR;
	ImageLayout ImageLayouts::max_enum = VK_IMAGE_LAYOUT_MAX_ENUM;

	std::string ImageLayouts::ToString(ImageLayout imageLayout)
	{
		const static std::unordered_map<ImageLayout, std::string> imageLayoutNames =
		{
			{ ImageLayouts::undefined, "undefined" },
			{ ImageLayouts::general, "general" },
			{ ImageLayouts::color_attachment_optimal, "color_attachment_optimal" },
			{ ImageLayouts::depth_stencil_attachment_optimal, "depth_stencil_attachment_optimal" },
			{ ImageLayouts::depth_stencil_read_only_optimal, "depth_stencil_read_only_optimal" },
			{ ImageLayouts::shader_read_only_optimal, "shader_read_only_optimal" },
			{ ImageLayouts::transfer_src_optimal, "transfer_src_optimal" },
			{ ImageLayouts::transfer_dst_optimal, "transfer_dst_optimal" },
			{ ImageLayouts::preinitialized, "preinitialized" },
			{ ImageLayouts::depth_read_only_stencil_attachment_optimal, "depth_read_only_stencil_attachment_optimal" },
			{ ImageLayouts::depth_attachment_stencil_read_only_optimal, "depth_attachment_stencil_read_only_optimal" },
			{ ImageLayouts::depth_attachment_optimal, "depth_attachment_optimal" },
			{ ImageLayouts::depth_read_only_optimal, "depth_read_only_optimal" },
			{ ImageLayouts::stencil_attachment_optimal, "stencil_attachment_optimal" },
			{ ImageLayouts::stencil_read_only_optimal, "stencil_read_only_optimal" },
			{ ImageLayouts::read_only_optimal, "read_only_optimal" },
			{ ImageLayouts::attachment_optimal, "attachment_optimal" },
			{ ImageLayouts::rendering_local_read, "rendering_local_read" },
			{ ImageLayouts::present_src_khr, "present_src_khr" },
			{ ImageLayouts::video_decode_dst_khr, "video_decode_dst_khr" },
			{ ImageLayouts::video_decode_src_khr, "video_decode_src_khr" },
			{ ImageLayouts::video_decode_dpb_khr, "video_decode_dpb_khr" },
			{ ImageLayouts::shared_present_khr, "shared_present_khr" },
			{ ImageLayouts::fragment_density_map_optimal_ext, "fragment_density_map_optimal_ext" },
			{ ImageLayouts::fragment_shading_rate_attachment_optimal_khr, "fragment_shading_rate_attachment_optimal_khr" },
			{ ImageLayouts::video_encode_dst_khr, "video_encode_dst_khr" },
			{ ImageLayouts::video_encode_src_khr, "video_encode_src_khr" },
			{ ImageLayouts::video_encode_dpb_khr, "video_encode_dpb_khr" },
			{ ImageLayouts::attachment_feedback_loop_optimal_ext, "attachment_feedback_loop_optimal_ext" },
			{ ImageLayouts::video_encode_quantization_map_khr, "video_encode_quantization_map_khr" },
			{ ImageLayouts::max_enum, "max_enum" }
		};

		auto it = imageLayoutNames.find(imageLayout);
		return it != imageLayoutNames.end() ? it->second : "unknown";
	}
}