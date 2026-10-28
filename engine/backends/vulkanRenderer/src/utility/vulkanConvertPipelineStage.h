#pragma once
#include "commonPipelineStage.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// emberCommon::PipelineStage -> VkPipelineStageFlags2:
	[[nodiscard]] VkPipelineStageFlags2 PipelineStageCommonToVulkan(emberCommon::PipelineStage stages);
}