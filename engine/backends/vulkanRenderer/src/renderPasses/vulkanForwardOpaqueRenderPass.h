#pragma once
#include "vulkanRenderPass.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class RenderTargetResources;



	/// <summary>
	/// Forward render pass for opaque, wireframe, and skybox materials.
	/// </summary>
	class ForwardOpaqueRenderPass : public RenderPass
	{
	public: // Methods:
		ForwardOpaqueRenderPass(const RenderTargetResources& renderTargets);
		~ForwardOpaqueRenderPass();

		// Non-copyable:
		ForwardOpaqueRenderPass(const ForwardOpaqueRenderPass&) = delete;
		ForwardOpaqueRenderPass& operator=(const ForwardOpaqueRenderPass&) = delete;

		// Movable:
		ForwardOpaqueRenderPass(ForwardOpaqueRenderPass&& other) noexcept = default;
		ForwardOpaqueRenderPass& operator=(ForwardOpaqueRenderPass&& other) noexcept = default;

	private: // Methods:
		void CreateRenderPass(const RenderTargetResources& renderTargets);
		void CreateFrameBuffers(const RenderTargetResources& renderTargets);
	};
}