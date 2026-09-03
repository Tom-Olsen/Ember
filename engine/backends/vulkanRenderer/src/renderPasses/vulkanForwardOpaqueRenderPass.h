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
	/// Forward render pass for opaque, wireframe, and skybox materials.
	/// </summary>
	class ForwardOpaqueRenderPass : public RenderPass
	{
	private: // Members:
		std::vector<RenderTexture2d*> m_pRenderTextures;
		std::vector<DepthTexture2d*> m_pDepthTextures;

	public: // Methods:
		ForwardOpaqueRenderPass(
			const std::vector<std::unique_ptr<RenderTexture2d>>& pRenderTextures,
			const std::vector<std::unique_ptr<DepthTexture2d>>& pDepthTextures);
		~ForwardOpaqueRenderPass();

		// Non-copyable:
		ForwardOpaqueRenderPass(const ForwardOpaqueRenderPass&) = delete;
		ForwardOpaqueRenderPass& operator=(const ForwardOpaqueRenderPass&) = delete;

		// Movable:
		ForwardOpaqueRenderPass(ForwardOpaqueRenderPass&& other) noexcept = default;
		ForwardOpaqueRenderPass& operator=(ForwardOpaqueRenderPass&& other) noexcept = default;

		// Getters:
		RenderTexture2d* GetRenderTexture(uint32_t frameIndex) const;
		DepthTexture2d* GetDepthTexture(uint32_t frameIndex) const;

	private: // Methods:
		void CreateRenderPass();
		void CreateFrameBuffers();
	};
}