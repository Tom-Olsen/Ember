#pragma once
#include "vulkanRenderPass.h"
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class DepthTexture2d;
	class RenderTexture2d;



	/// <summary>
	/// Forward render pass for transparent materials.
	/// </summary>
	class ForwardTransparentRenderPass : public RenderPass
	{
	private: // Members:
		std::vector<RenderTexture2d*> m_pRenderTextures;
		std::vector<DepthTexture2d*> m_pDepthTextures;

	public: // Methods:
		ForwardTransparentRenderPass(
			const std::vector<std::unique_ptr<RenderTexture2d>>& pRenderTextures,
			const std::vector<std::unique_ptr<DepthTexture2d>>& pDepthTextures);
		~ForwardTransparentRenderPass();

		// Non-copyable:
		ForwardTransparentRenderPass(const ForwardTransparentRenderPass&) = delete;
		ForwardTransparentRenderPass& operator=(const ForwardTransparentRenderPass&) = delete;

		// Movable:
		ForwardTransparentRenderPass(ForwardTransparentRenderPass&& other) noexcept = default;
		ForwardTransparentRenderPass& operator=(ForwardTransparentRenderPass&& other) noexcept = default;

		// Getters:
		RenderTexture2d* GetRenderTexture(uint32_t frameIndex) const;
		DepthTexture2d* GetDepthTexture(uint32_t frameIndex) const;

	private: // Methods:
		void CreateRenderPass();
		void CreateFrameBuffers();
	};
}