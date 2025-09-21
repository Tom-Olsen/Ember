#include "vulkanPipelineStage.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	PipelineStage PipelineStages::topOfPipe = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
	PipelineStage PipelineStages::transfer = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
	PipelineStage PipelineStages::computeShader = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
	PipelineStage PipelineStages::vertexAttributeInput = VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT;
	PipelineStage PipelineStages::vertexInput = VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT;
	PipelineStage PipelineStages::vertexShader = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
	PipelineStage PipelineStages::earlyFragmentTest = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT;
	PipelineStage PipelineStages::lateFragmentTest = VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
	PipelineStage PipelineStages::fragmentShader = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
	PipelineStage PipelineStages::colorAttachmentOutput = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
	PipelineStage PipelineStages::copy = VK_PIPELINE_STAGE_2_COPY_BIT;
	PipelineStage PipelineStages::none = VK_PIPELINE_STAGE_2_NONE;
	PipelineStage PipelineStages::bottomOfPipe = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
	PipelineStage PipelineStages::allCommands = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;

	std::string PipelineStages::ToString(PipelineStage pipelineStage)
	{
		if (pipelineStage == 0)
			return "none";

		std::string result;

		auto append = [&](PipelineStage bit, std::string name)
		{
			if (pipelineStage & bit)
			{
				if (!result.empty()) result += " | ";
				result += name;
			}
		};

		append(PipelineStages::topOfPipe, "topOfPipe");
		append(PipelineStages::transfer, "transfer");
		append(PipelineStages::computeShader, "computeShader");
		append(PipelineStages::vertexAttributeInput, "vertexAttributeInput");
		append(PipelineStages::vertexInput, "vertexInput");
		append(PipelineStages::vertexShader, "vertexShader");
		append(PipelineStages::earlyFragmentTest, "earlyFragmentTest");
		append(PipelineStages::lateFragmentTest, "lateFragmentTest");
		append(PipelineStages::fragmentShader, "fragmentShader");
		append(PipelineStages::colorAttachmentOutput, "colorAttachmentOutput");
		append(PipelineStages::copy, "copy");
		append(PipelineStages::none, "none");
		append(PipelineStages::bottomOfPipe, "bottomOfPipe");
		append(PipelineStages::allCommands, "allCommands");

		return result.empty() ? "unknown" : result;
	}
}