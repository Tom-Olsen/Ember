#pragma once
#include <string>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Add more Stages when needed: https://registry.khronos.org/vulkan/specs/latest/man/html/VkAccessFlagBits.html
	namespace pipelineStage
	{
		constexpr VkPipelineStageFlags2 topOfPipe = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
		constexpr VkPipelineStageFlags2 transfer = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
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
		constexpr VkPipelineStageFlags2 allCommands = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	}
}