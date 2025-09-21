#include "vulkanDescriptorType.h"
#include <unordered_map>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	DescriptorType DescriptorTypes::sampler = VK_DESCRIPTOR_TYPE_SAMPLER;
	DescriptorType DescriptorTypes::combined_image_sampler = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	DescriptorType DescriptorTypes::sampled_image = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	DescriptorType DescriptorTypes::storage_image = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	DescriptorType DescriptorTypes::uniform_texel_buffer = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
	DescriptorType DescriptorTypes::storage_texel_buffer = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
	DescriptorType DescriptorTypes::uniform_buffer = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	DescriptorType DescriptorTypes::storage_buffer = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	DescriptorType DescriptorTypes::uniform_buffer_dynamic = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	DescriptorType DescriptorTypes::storage_buffer_dynamic = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
	DescriptorType DescriptorTypes::input_attachment = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	DescriptorType DescriptorTypes::inline_uniform_block = VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK;
	DescriptorType DescriptorTypes::acceleration_structure_khr = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
	DescriptorType DescriptorTypes::acceleration_structure_nv = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV;
	DescriptorType DescriptorTypes::sample_weight_image_qcom = VK_DESCRIPTOR_TYPE_SAMPLE_WEIGHT_IMAGE_QCOM;
	DescriptorType DescriptorTypes::block_match_image_qcom = VK_DESCRIPTOR_TYPE_BLOCK_MATCH_IMAGE_QCOM;
	DescriptorType DescriptorTypes::mutable_ext = VK_DESCRIPTOR_TYPE_MUTABLE_EXT;
	DescriptorType DescriptorTypes::max_enum = VK_DESCRIPTOR_TYPE_MAX_ENUM;

	std::string DescriptorTypes::ToString(DescriptorType descriptorType)
	{
		const static std::unordered_map<DescriptorType, std::string> descriptorTypeNames =
		{
			{ DescriptorTypes::sampler, "sampler" },
			{ DescriptorTypes::combined_image_sampler, "combined_image_sampler" },
			{ DescriptorTypes::sampled_image, "sampled_image" },
			{ DescriptorTypes::storage_image, "storage_image" },
			{ DescriptorTypes::uniform_texel_buffer, "uniform_texel_buffer" },
			{ DescriptorTypes::storage_texel_buffer, "storage_texel_buffer" },
			{ DescriptorTypes::uniform_buffer, "uniform_buffer" },
			{ DescriptorTypes::storage_buffer, "storage_buffer" },
			{ DescriptorTypes::uniform_buffer_dynamic, "uniform_buffer_dynamic" },
			{ DescriptorTypes::storage_buffer_dynamic, "storage_buffer_dynamic" },
			{ DescriptorTypes::input_attachment, "input_attachment" },
			{ DescriptorTypes::inline_uniform_block, "inline_uniform_block" },
			{ DescriptorTypes::acceleration_structure_khr, "acceleration_structure_khr" },
			{ DescriptorTypes::acceleration_structure_nv, "acceleration_structure_nv" },
			{ DescriptorTypes::sample_weight_image_qcom, "sample_weight_image_qcom" },
			{ DescriptorTypes::block_match_image_qcom, "block_match_image_qcom" },
			{ DescriptorTypes::mutable_ext, "mutable_ext" },
			{ DescriptorTypes::max_enum, "max_enum" }
		};

		auto it = descriptorTypeNames.find(descriptorType);
		return it != descriptorTypeNames.end() ? it->second : "unknown";
	}
}