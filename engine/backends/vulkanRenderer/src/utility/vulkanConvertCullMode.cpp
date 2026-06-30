#include "vulkanConvertCullMode.h"
#include "vulkanDrawCall.h"
#include <stdexcept>



namespace vulkanRendererBackend
{
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
			case emberCommon::CullMode::pipelineDefault:
				return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
			default:
				throw std::runtime_error("CullModeCommonToVulkan(...) failed. Unsupported cull mode.");
		}
	}
	VkCullModeFlagBits ResolveCullMode(VkCullModeFlagBits cullMode, VkCullModeFlagBits fallbackCullMode)
	{
		if (cullMode == VK_CULL_MODE_FLAG_BITS_MAX_ENUM)
			return fallbackCullMode;
		return cullMode;
	}
	VkCullModeFlagBits RenderModeToCullMode(emberCommon::RenderMode renderMode)
	{
		switch (renderMode)
		{
			case emberCommon::RenderMode::opaque:
				return VK_CULL_MODE_BACK_BIT;
			case emberCommon::RenderMode::transparent:
				return VK_CULL_MODE_NONE;
			case emberCommon::RenderMode::skybox:
				return VK_CULL_MODE_BACK_BIT;
			case emberCommon::RenderMode::wireframe:
				return VK_CULL_MODE_NONE;
			default:
				throw std::runtime_error("RenderModeToCullMode(...) failed. Unsupported render mode.");
		}
	}
}