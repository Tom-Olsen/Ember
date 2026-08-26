#pragma once
#include "vulkanRenderPass.h"
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class DeferredGeometryRenderPass;
	class DepthTexture2d;
	class GBufferTexture2d;
	class RenderTexture2d;



	/// <summary>
	/// Deferred lighting render pass.
	/// Consumes the G-buffer and produces the lit scene color.
	/// </summary>
	class DeferredLightingRenderPass : public RenderPass
	{
	private: // Members:
		std::vector<RenderTexture2d*> m_pSceneColorTextures;
		std::vector<GBufferTexture2d*> m_pAlbedoTextures;
		std::vector<GBufferTexture2d*> m_pNormalTextures;
		std::vector<GBufferTexture2d*> m_pMaterialTextures;
		std::vector<DepthTexture2d*> m_pDepthTextures;

	public: // Methods:
		DeferredLightingRenderPass(
			const std::vector<std::unique_ptr<RenderTexture2d>>& pSceneColorTextures,
			const std::vector<std::unique_ptr<DepthTexture2d>>& pDepthTextures,
			const DeferredGeometryRenderPass& deferredGeometryRenderPass);
		~DeferredLightingRenderPass();

		// Non-copyable:
		DeferredLightingRenderPass(const DeferredLightingRenderPass&) = delete;
		DeferredLightingRenderPass& operator=(const DeferredLightingRenderPass&) = delete;

		// Movable:
		DeferredLightingRenderPass(DeferredLightingRenderPass&& other) noexcept = default;
		DeferredLightingRenderPass& operator=(DeferredLightingRenderPass&& other) noexcept = default;

		// Getters:
		RenderTexture2d* GetSceneColorTexture(uint32_t frameIndex) const;
		GBufferTexture2d* GetAlbedoTexture(uint32_t frameIndex) const;
		GBufferTexture2d* GetNormalTexture(uint32_t frameIndex) const;
		GBufferTexture2d* GetMaterialTexture(uint32_t frameIndex) const;
		DepthTexture2d* GetDepthTexture(uint32_t frameIndex) const;

	private: // Methods:
		void CreateRenderPass();
		void CreateFrameBuffers();
	};
}