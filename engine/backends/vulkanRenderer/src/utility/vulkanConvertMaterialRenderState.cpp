#include "vulkanConvertMaterialRenderState.h"
#include <stdexcept>



namespace vulkanRendererBackend
{
	VkBlendFactor BlendFactorCommonToVulkan(emberCommon::BlendFactor blendFactor)
	{
		switch (blendFactor)
		{
			case emberCommon::BlendFactor::zero:
				return VK_BLEND_FACTOR_ZERO;
			case emberCommon::BlendFactor::one:
				return VK_BLEND_FACTOR_ONE;
			case emberCommon::BlendFactor::srcColor:
				return VK_BLEND_FACTOR_SRC_COLOR;
			case emberCommon::BlendFactor::oneMinusSrcColor:
				return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
			case emberCommon::BlendFactor::dstColor:
				return VK_BLEND_FACTOR_DST_COLOR;
			case emberCommon::BlendFactor::oneMinusDstColor:
				return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
			case emberCommon::BlendFactor::srcAlpha:
				return VK_BLEND_FACTOR_SRC_ALPHA;
			case emberCommon::BlendFactor::oneMinusSrcAlpha:
				return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			case emberCommon::BlendFactor::dstAlpha:
				return VK_BLEND_FACTOR_DST_ALPHA;
			case emberCommon::BlendFactor::oneMinusDstAlpha:
				return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
			default:
				throw std::runtime_error("BlendFactorCommonToVulkan(...) failed. Unsupported blend factor.");
		}
	}
	VkBlendOp BlendOpCommonToVulkan(emberCommon::BlendOp blendOp)
	{
		switch (blendOp)
		{
			case emberCommon::BlendOp::add:
				return VK_BLEND_OP_ADD;
			case emberCommon::BlendOp::subtract:
				return VK_BLEND_OP_SUBTRACT;
			case emberCommon::BlendOp::reverseSubtract:
				return VK_BLEND_OP_REVERSE_SUBTRACT;
			case emberCommon::BlendOp::min:
				return VK_BLEND_OP_MIN;
			case emberCommon::BlendOp::max:
				return VK_BLEND_OP_MAX;
			default:
				throw std::runtime_error("BlendOpCommonToVulkan(...) failed. Unsupported blend op.");
		}
	}
	VkCullModeFlagBits CullModeCommonToVulkan(emberCommon::CullMode cullMode)
	{
		switch (cullMode)
		{
			case emberCommon::CullMode::none:
				return VK_CULL_MODE_NONE;
			case emberCommon::CullMode::front:
				return VK_CULL_MODE_FRONT_BIT;
			case emberCommon::CullMode::back:
				return VK_CULL_MODE_BACK_BIT;
			case emberCommon::CullMode::both:
				return VK_CULL_MODE_FRONT_AND_BACK;
			default:
				throw std::runtime_error("CullModeCommonToVulkan(...) failed. Unsupported cull mode.");
		}
	}
	VkColorComponentFlags ColorWriteMaskCommonToVulkan(uint32_t colorWriteMask)
	{
		VkColorComponentFlags vkColorWriteMask = 0;
		if (colorWriteMask & emberCommon::ColorComponent::red)
			vkColorWriteMask |= VK_COLOR_COMPONENT_R_BIT;
		if (colorWriteMask & emberCommon::ColorComponent::green)
			vkColorWriteMask |= VK_COLOR_COMPONENT_G_BIT;
		if (colorWriteMask & emberCommon::ColorComponent::blue)
			vkColorWriteMask |= VK_COLOR_COMPONENT_B_BIT;
		if (colorWriteMask & emberCommon::ColorComponent::alpha)
			vkColorWriteMask |= VK_COLOR_COMPONENT_A_BIT;
		return vkColorWriteMask;
	}
	VkCompareOp CompareOpCommonToVulkan(emberCommon::CompareOp compareOp)
	{
		switch (compareOp)
		{
			case emberCommon::CompareOp::never:
				return VK_COMPARE_OP_NEVER;
			case emberCommon::CompareOp::less:
				return VK_COMPARE_OP_LESS;
			case emberCommon::CompareOp::equal:
				return VK_COMPARE_OP_EQUAL;
			case emberCommon::CompareOp::lessOrEqual:
				return VK_COMPARE_OP_LESS_OR_EQUAL;
			case emberCommon::CompareOp::greater:
				return VK_COMPARE_OP_GREATER;
			case emberCommon::CompareOp::notEqual:
				return VK_COMPARE_OP_NOT_EQUAL;
			case emberCommon::CompareOp::greaterOrEqual:
				return VK_COMPARE_OP_GREATER_OR_EQUAL;
			case emberCommon::CompareOp::always:
				return VK_COMPARE_OP_ALWAYS;
			default:
				throw std::runtime_error("CompareOpCommonToVulkan(...) failed. Unsupported compare op.");
		}
	}
	VkFrontFace FrontFaceCommonToVulkan(emberCommon::FrontFace frontFace)
	{
		switch (frontFace)
		{
			case emberCommon::FrontFace::clockwise:
				return VK_FRONT_FACE_CLOCKWISE;
			case emberCommon::FrontFace::counterClockwise:
				return VK_FRONT_FACE_COUNTER_CLOCKWISE;
			default:
				throw std::runtime_error("FrontFaceCommonToVulkan(...) failed. Unsupported front face.");
		}
	}
	VkPolygonMode PolygonModeCommonToVulkan(emberCommon::PolygonMode polygonMode)
	{
		switch (polygonMode)
		{
			case emberCommon::PolygonMode::fill:
				return VK_POLYGON_MODE_FILL;
			case emberCommon::PolygonMode::line:
				return VK_POLYGON_MODE_LINE;
			case emberCommon::PolygonMode::point:
				return VK_POLYGON_MODE_POINT;
			default:
				throw std::runtime_error("PolygonModeCommonToVulkan(...) failed. Unsupported polygon mode.");
		}
	}
}
