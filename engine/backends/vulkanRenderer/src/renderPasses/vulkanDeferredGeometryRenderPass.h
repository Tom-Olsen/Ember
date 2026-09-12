#pragma once
#include "vulkanRenderPass.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class RenderTargetResources;



	/// <summary>
	/// Deferred geometry render pass.
	/// Writes the scene geometry data into G-buffers.
	/// </summary>
	class DeferredGeometryRenderPass : public RenderPass
	{
	public: // Methods:
		DeferredGeometryRenderPass(const RenderTargetResources& renderTargets);
		~DeferredGeometryRenderPass();

		// Non-copyable:
		DeferredGeometryRenderPass(const DeferredGeometryRenderPass&) = delete;
		DeferredGeometryRenderPass& operator=(const DeferredGeometryRenderPass&) = delete;

		// Movable:
		DeferredGeometryRenderPass(DeferredGeometryRenderPass&& other) noexcept = default;
		DeferredGeometryRenderPass& operator=(DeferredGeometryRenderPass&& other) noexcept = default;

	private: // Methods:
		void CreateRenderPass(const RenderTargetResources& renderTargets);
		void CreateFrameBuffers(const RenderTargetResources& renderTargets);
	};
}