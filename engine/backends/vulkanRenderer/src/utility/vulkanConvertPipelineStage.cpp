#include "vulkanConvertPipelineStage.h"
#include <stdexcept>



namespace vulkanRendererBackend
{
	// emberCommon::PipelineStage -> VkPipelineStageFlags2:
	VkPipelineStageFlags2 PipelineStageCommonToVulkan(emberCommon::PipelineStage stages)
	{
		using emberCommon::HasFlag;
		using emberCommon::PipelineStage;
		constexpr PipelineStage knownStages = PipelineStage::vertexShader | PipelineStage::fragmentShader | PipelineStage::computeShader | PipelineStage::copy | PipelineStage::allGraphics | PipelineStage::allCommands;
		if ((static_cast<uint32_t>(stages) & ~static_cast<uint32_t>(knownStages)) != 0)
			throw std::invalid_argument("PipelineStageCommonToVulkan(...) contains unsupported stages.");

		VkPipelineStageFlags2 result = VK_PIPELINE_STAGE_2_NONE;
		if (HasFlag(stages, PipelineStage::vertexShader))
			result |= VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
		if (HasFlag(stages, PipelineStage::fragmentShader))
			result |= VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
		if (HasFlag(stages, PipelineStage::computeShader))
			result |= VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
		if (HasFlag(stages, PipelineStage::copy))
			result |= VK_PIPELINE_STAGE_2_COPY_BIT;
		if (HasFlag(stages, PipelineStage::allGraphics))
			result |= VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
		if (HasFlag(stages, PipelineStage::allCommands))
			result |= VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		return result;
	}
}