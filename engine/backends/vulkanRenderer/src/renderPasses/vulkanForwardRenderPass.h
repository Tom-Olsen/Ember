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
	/// Basic forward render pass.
	/// </summary>
	class ForwardRenderPass : public RenderPass
	{
	private: // Members:
		std::vector<RenderTexture2d*> m_pRenderTextures;
		std::vector<RenderTexture2d*> m_pSecondaryRenderTextures;
		std::vector<DepthTexture2d*> m_pDepthTextures;

	public: // Methods:
		ForwardRenderPass(
			const std::vector<std::unique_ptr<RenderTexture2d>>& pRenderTextures,
			const std::vector<std::unique_ptr<RenderTexture2d>>& pSecondaryRenderTextures,
			const std::vector<std::unique_ptr<DepthTexture2d>>& pDepthTextures);
		~ForwardRenderPass();

		// Non-copyable:
		ForwardRenderPass(const ForwardRenderPass&) = delete;
		ForwardRenderPass& operator=(const ForwardRenderPass&) = delete;

		// Movable:
		ForwardRenderPass(ForwardRenderPass&& other) noexcept = default;
		ForwardRenderPass& operator=(ForwardRenderPass&& other) noexcept = default;

		// Getters:
		RenderTexture2d* GetRenderTexture(uint32_t frameIndex) const;
		RenderTexture2d* GetSecondaryRenderTexture(uint32_t frameIndex) const;
		DepthTexture2d* GetDepthTexture(uint32_t frameIndex) const;

	private: // Methods:
		void CreateRenderPass();
		void CreateFrameBuffers();
	};
}