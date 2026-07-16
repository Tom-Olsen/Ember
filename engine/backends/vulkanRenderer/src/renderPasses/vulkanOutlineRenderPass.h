#pragma once
#include "vulkanRenderPass.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	/// <summary>
	/// Runs parallel to shadow/forward.
    /// Renders a binary mask into its own frameBuffer which then gets dilated in the postRender compute pass.
    /// Outline = DilationMask - Mask, which is then compositted on the final image before gizmos.
	/// </summary>
	class OutlineRenderPass : public RenderPass
    {

    }
}