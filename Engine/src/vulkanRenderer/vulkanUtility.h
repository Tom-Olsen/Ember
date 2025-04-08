#ifndef __INCLUDE_GUARD_vulkanUtility_h__
#define __INCLUDE_GUARD_vulkanUtility_h__
#include <string>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	namespace vulkanObjToString
	{
		std::string VkAccessFlagBits2ToString(VkAccessFlagBits2 vkAccessFlagBits2);
		std::string VkDescriptorTypeToString(VkDescriptorType vkDescriptorType);
		std::string VkFormatToString(VkFormat vkFormat);
		std::string VkImageLayoutToString(VkImageLayout vkImageView);
		std::string VkImageViewTypeToString(VkImageViewType vkImageViewType);
		std::string VkShaderStageFlagsToString(VkShaderStageFlags vkShaderStageFlags);
		std::string VkVertexInputAttributeDescriptionToString(VkVertexInputAttributeDescription vkVertexInputAttributeDescription);
		std::string VkVertexInputRateToString(VkVertexInputRate vkVertexInputRate);
	}

	// Add more Stages when needed: https://registry.khronos.org/vulkan/specs/latest/man/html/VkAccessFlagBits.html
	namespace PipelineStage
	{
		constexpr VkPipelineStageFlags2 topOfPipe = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
		constexpr VkPipelineStageFlags2 computeShader = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
		constexpr VkPipelineStageFlags2 vertexAttributeInput = VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT;
		constexpr VkPipelineStageFlags2 vertexInput = VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT;
		constexpr VkPipelineStageFlags2 vertexShader = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
		constexpr VkPipelineStageFlags2 earlyFragmentTest = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT;
		constexpr VkPipelineStageFlags2 fragmentShader = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
		constexpr VkPipelineStageFlags2 colorAttachmentOutput = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
		constexpr VkPipelineStageFlags2 copy = VK_PIPELINE_STAGE_2_COPY_BIT;
		constexpr VkPipelineStageFlags2 none = VK_PIPELINE_STAGE_2_NONE;
		constexpr VkPipelineStageFlags2 bottomOfPipe = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
	}
	namespace AccessMask
	{
		namespace TopOfPipe				// VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT
		{
			constexpr VkAccessFlagBits2 none = VK_ACCESS_2_NONE;
		}

		namespace ComputeShader			// VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT
		{
			constexpr VkAccessFlagBits2 uniformRead = VK_ACCESS_2_UNIFORM_READ_BIT;
			constexpr VkAccessFlagBits2 shaderRead = VK_ACCESS_2_SHADER_READ_BIT;
			constexpr VkAccessFlagBits2 shaderWrite = VK_ACCESS_2_SHADER_WRITE_BIT;
			constexpr VkAccessFlagBits2 sampledRead = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;
			constexpr VkAccessFlagBits2 storageRead = VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
			constexpr VkAccessFlagBits2 storageWrite = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
			constexpr VkAccessFlagBits2 accelerationStructureRead = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR;
			constexpr VkAccessFlagBits2 descriptorBufferRead = VK_ACCESS_2_DESCRIPTOR_BUFFER_READ_BIT_EXT;
			constexpr VkAccessFlagBits2 none = VK_ACCESS_2_NONE;
		}

		namespace VertexAttributeInput	// VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT
		{
			constexpr VkAccessFlagBits2 vertexAttributeRead = VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;
			constexpr VkAccessFlagBits2 none = VK_ACCESS_2_NONE;
		}

		namespace VertexInput			// VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT
		{
			constexpr VkAccessFlagBits2 indexRead = VK_ACCESS_2_INDEX_READ_BIT;
			constexpr VkAccessFlagBits2 vertexAttributeRead = VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;
			constexpr VkAccessFlagBits2 none = VK_ACCESS_2_NONE;
		}

		namespace VertexShader			// VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT
		{
			constexpr VkAccessFlagBits2 uniformRead = VK_ACCESS_2_UNIFORM_READ_BIT;
			constexpr VkAccessFlagBits2 shaderRead = VK_ACCESS_2_SHADER_READ_BIT;
			constexpr VkAccessFlagBits2 shaderWrite = VK_ACCESS_2_SHADER_WRITE_BIT;
			constexpr VkAccessFlagBits2 sampledRead = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;
			constexpr VkAccessFlagBits2 storageRead = VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
			constexpr VkAccessFlagBits2 storageWrite = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
			constexpr VkAccessFlagBits2 accelerationStructureRead = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR;
			constexpr VkAccessFlagBits2 descriptorBufferRead = VK_ACCESS_2_DESCRIPTOR_BUFFER_READ_BIT_EXT;
			constexpr VkAccessFlagBits2 none = VK_ACCESS_2_NONE;
		}

		namespace EarlyFragmentTest		// VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT
		{
			constexpr VkAccessFlagBits2 depthStencilAttachmentRead = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
			constexpr VkAccessFlagBits2 depthStencilAttachmentWrite = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			constexpr VkAccessFlagBits2 none = VK_ACCESS_2_NONE;
		}

		namespace FragmentShader		// VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT
		{
			constexpr VkAccessFlagBits2 uniformRead = VK_ACCESS_2_UNIFORM_READ_BIT;
			constexpr VkAccessFlagBits2 shaderRead = VK_ACCESS_2_SHADER_READ_BIT;
			constexpr VkAccessFlagBits2 shaderWrite = VK_ACCESS_2_SHADER_WRITE_BIT;
			constexpr VkAccessFlagBits2 colorAttachmentRead = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT;
			constexpr VkAccessFlagBits2 depthStencilAttachmentRead = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
			constexpr VkAccessFlagBits2 sampledRead = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;
			constexpr VkAccessFlagBits2 storageRead = VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
			constexpr VkAccessFlagBits2 storageWrite = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
			constexpr VkAccessFlagBits2 accelerationStructureRead = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR;
			constexpr VkAccessFlagBits2 descriptorBufferRead = VK_ACCESS_2_DESCRIPTOR_BUFFER_READ_BIT_EXT;
			constexpr VkAccessFlagBits2 none = VK_ACCESS_2_NONE;
		}

		namespace ColorAttachmentOutput	// VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT
		{
			constexpr VkAccessFlagBits2 colorAttachmentRead = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT;
			constexpr VkAccessFlagBits2 colorAttachmentWrite = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
			constexpr VkAccessFlagBits2 none = VK_ACCESS_2_NONE;
		}

		namespace Copy					// VK_PIPELINE_STAGE_2_COPY_BIT
		{
			constexpr VkAccessFlagBits2 transferRead = VK_ACCESS_2_TRANSFER_READ_BIT;
			constexpr VkAccessFlagBits2 transferWrite = VK_ACCESS_2_TRANSFER_WRITE_BIT;
			constexpr VkAccessFlagBits2 none = VK_ACCESS_2_NONE;
		}

		namespace None					// VK_PIPELINE_STAGE_2_NONE
		{
			constexpr VkAccessFlagBits2 none = VK_ACCESS_2_NONE;
		}

		namespace BottomOfPipe			// VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT
		{
			constexpr VkAccessFlagBits2 none = VK_ACCESS_2_NONE;
		}
	}
}



#endif // __INCLUDE_GUARD_vulkanUtility_h__