#pragma once
#include "vulkanRenderPass.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class RenderTargetResources;



	/// <summary>
	/// Shadow render pass.
	/// One layer for eacg light source.
	/// </summary>
	class ShadowRenderPass : public RenderPass
	{
	public: // Methods:
		ShadowRenderPass(const RenderTargetResources& renderTargets);
		~ShadowRenderPass();

		// Non-copyable:
		ShadowRenderPass(const ShadowRenderPass&) = delete;
		ShadowRenderPass& operator=(const ShadowRenderPass&) = delete;

		// Movable:
		ShadowRenderPass(ShadowRenderPass&& other) noexcept = default;
		ShadowRenderPass& operator=(ShadowRenderPass&& other) noexcept = default;

	private: // Methods:
		void CreateRenderpass(const RenderTargetResources& renderTargets);
		void CreateFramebuffers(const RenderTargetResources& renderTargets);
	};
}