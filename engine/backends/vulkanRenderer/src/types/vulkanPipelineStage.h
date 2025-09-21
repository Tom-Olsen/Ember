#pragma once
#include <cstdint>
#include <string>



namespace vulkanRendererBackend
{
	using PipelineStage = uint64_t;
	struct PipelineStages
	{
		static PipelineStage topOfPipe;
		static PipelineStage transfer;
		static PipelineStage computeShader;
		static PipelineStage vertexAttributeInput;
		static PipelineStage vertexInput;
		static PipelineStage vertexShader;
		static PipelineStage earlyFragmentTest;
		static PipelineStage lateFragmentTest;
		static PipelineStage fragmentShader;
		static PipelineStage colorAttachmentOutput;
		static PipelineStage copy;
		static PipelineStage none;
		static PipelineStage bottomOfPipe;
		static PipelineStage allCommands;
		static std::string ToString(PipelineStage pipelineStage);
	};
}