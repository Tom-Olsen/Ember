#pragma once
#include "commonCullMode.h"
#include "commonMaterialRenderState.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	[[nodiscard]] VkBlendFactor BlendFactorCommonToVulkan(emberCommon::BlendFactor blendFactor);
	[[nodiscard]] VkBlendOp BlendOpCommonToVulkan(emberCommon::BlendOp blendOp);
	[[nodiscard]] VkCullModeFlagBits CullModeCommonToVulkan(emberCommon::CullMode cullMode);
	[[nodiscard]] VkColorComponentFlags ColorWriteMaskCommonToVulkan(uint32_t colorWriteMask);
	[[nodiscard]] VkCompareOp CompareOpCommonToVulkan(emberCommon::CompareOp compareOp);
	[[nodiscard]] VkFrontFace FrontFaceCommonToVulkan(emberCommon::FrontFace frontFace);
	[[nodiscard]] VkPolygonMode PolygonModeCommonToVulkan(emberCommon::PolygonMode polygonMode);
}