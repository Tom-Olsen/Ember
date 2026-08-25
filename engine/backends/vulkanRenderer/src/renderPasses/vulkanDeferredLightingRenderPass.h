#pragma once
#include "vulkanRenderPass.h"
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class RenderTexture2d;



	/// <summary>
	/// Deferred lighting render pass.
	/// Consumes the G-buffer and produces the lit scene color.
	/// </summary>
	class DeferredLightingRenderPass : public RenderPass
	{
	public: // Methods:
		DeferredLightingRenderPass(uint32_t renderWidth, uint32_t renderHeight);
		~DeferredLightingRenderPass();

		// Non-copyable:
		DeferredLightingRenderPass(const DeferredLightingRenderPass&) = delete;
		DeferredLightingRenderPass& operator=(const DeferredLightingRenderPass&) = delete;

		// Movable:
		DeferredLightingRenderPass(DeferredLightingRenderPass&& other) noexcept = default;
		DeferredLightingRenderPass& operator=(DeferredLightingRenderPass&& other) noexcept = default;

	private: // Methods:
		void CreateRenderPass();
		void CreateFrameBuffers();
	};
}