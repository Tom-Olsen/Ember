#pragma once
#include "vulkanRenderPass.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class RenderTargetResources;



	/// <summary>
	/// Runs parallel to shadow/forward/postProcessing.
    /// Final result is then overlayed ontop of forwardRenderpass renterTextures.
	/// </summary>
	class GizmoRenderPass : public RenderPass
	{
	public: // Methods:
		GizmoRenderPass(const RenderTargetResources& renderTargets);
		~GizmoRenderPass();

		// Non-copyable:
		GizmoRenderPass(const GizmoRenderPass&) = delete;
		GizmoRenderPass& operator=(const GizmoRenderPass&) = delete;

		// Movable:
		GizmoRenderPass(GizmoRenderPass&& other) noexcept = default;
		GizmoRenderPass& operator=(GizmoRenderPass&& other) noexcept = default;

	private: // Methods:
		void CreateRenderPass(const RenderTargetResources& renderTargets);
		void CreateFrameBuffers(const RenderTargetResources& renderTargets);
	};
}