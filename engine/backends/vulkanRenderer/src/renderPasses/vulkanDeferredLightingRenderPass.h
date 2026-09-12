#pragma once
#include "vulkanRenderPass.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class RenderTargetResources;



	/// <summary>
	/// Deferred lighting render pass.
	/// Consumes the G-buffer and produces the lit scene color.
	/// </summary>
	class DeferredLightingRenderPass : public RenderPass
	{
	public: // Methods:
		DeferredLightingRenderPass(const RenderTargetResources& renderTargets);
		~DeferredLightingRenderPass();

		// Non-copyable:
		DeferredLightingRenderPass(const DeferredLightingRenderPass&) = delete;
		DeferredLightingRenderPass& operator=(const DeferredLightingRenderPass&) = delete;

		// Movable:
		DeferredLightingRenderPass(DeferredLightingRenderPass&& other) noexcept = default;
		DeferredLightingRenderPass& operator=(DeferredLightingRenderPass&& other) noexcept = default;

	private: // Methods:
		void CreateRenderPass(const RenderTargetResources& renderTargets);
		void CreateFrameBuffers(const RenderTargetResources& renderTargets);
	};
}