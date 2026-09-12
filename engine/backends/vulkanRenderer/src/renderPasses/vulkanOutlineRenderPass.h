#pragma once
#include "vulkanRenderPass.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class RenderTargetResources;



	/// <summary>
	/// Renders a binary mask into its own framebuffer.
	/// Mid-render compute expands the mask before post-render compute composites it onto the forward image.
	/// </summary>
	class OutlineRenderPass : public RenderPass
	{
	public: // Methods:
		OutlineRenderPass(const RenderTargetResources& renderTargets);
		~OutlineRenderPass();

		// Non-copyable:
		OutlineRenderPass(const OutlineRenderPass&) = delete;
		OutlineRenderPass& operator=(const OutlineRenderPass&) = delete;

		// Movable:
		OutlineRenderPass(OutlineRenderPass&& other) noexcept = default;
		OutlineRenderPass& operator=(OutlineRenderPass&& other) noexcept = default;

	private: // Methods:
		void CreateRenderPass(const RenderTargetResources& renderTargets);
		void CreateFrameBuffers(const RenderTargetResources& renderTargets);
	};
}