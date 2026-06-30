#pragma once
#include "commonCullMode.h"
#include "commonPipelineState.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	[[nodiscard]] VkCullModeFlagBits CullModeCommonToVulkan(emberCommon::CullMode cullMode);
	[[nodiscard]] VkCullModeFlagBits ResolveCullMode(VkCullModeFlagBits cullMode, VkCullModeFlagBits fallbackCullMode);
	[[nodiscard]] VkCullModeFlagBits RenderModeToCullMode(emberCommon::RenderMode renderMode);
}