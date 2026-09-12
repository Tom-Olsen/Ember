#pragma once
#include "vulkanRenderPass.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class RenderTargetResources;



	/// <summary>
	/// Forward render pass for transparent materials.
	/// </summary>
	class ForwardTransparentRenderPass : public RenderPass
	{
	private: // Members:
		uint32_t m_frameCount;

	public: // Methods:
		ForwardTransparentRenderPass(const RenderTargetResources& renderTargets);
		~ForwardTransparentRenderPass();

		// Non-copyable:
		ForwardTransparentRenderPass(const ForwardTransparentRenderPass&) = delete;
		ForwardTransparentRenderPass& operator=(const ForwardTransparentRenderPass&) = delete;

		// Movable:
		ForwardTransparentRenderPass(ForwardTransparentRenderPass&& other) noexcept = default;
		ForwardTransparentRenderPass& operator=(ForwardTransparentRenderPass&& other) noexcept = default;

		// Getters:
		const VkFramebuffer& GetFramebuffer(uint32_t frameIndex, uint32_t sceneColorIndex) const;

	private: // Methods:
		void CreateRenderPass(const RenderTargetResources& renderTargets);
		void CreateFrameBuffers(const RenderTargetResources& renderTargets);
	};
}