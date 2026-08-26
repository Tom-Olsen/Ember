#pragma once
#include "vulkanRenderPass.h"
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class DepthTexture2d;
	class GBufferTexture2d;



	/// <summary>
	/// Deferred geometry render pass.
	/// Writes the scene geometry data into G-buffers.
	/// </summary>
	class DeferredGeometryRenderPass : public RenderPass
	{
	private: // Members:
		// G-buffer attachments:
		std::vector<std::unique_ptr<GBufferTexture2d>> m_pAlbedoTextures;
		std::vector<std::unique_ptr<GBufferTexture2d>> m_pNormalTextures;
		std::vector<std::unique_ptr<GBufferTexture2d>> m_pMaterialTextures;
		std::vector<DepthTexture2d*> m_pDepthTextures;

	public: // Methods:
		DeferredGeometryRenderPass(uint32_t renderWidth, uint32_t renderHeight, const std::vector<std::unique_ptr<DepthTexture2d>>& pDepthTextures);
		~DeferredGeometryRenderPass();

		// Non-copyable:
		DeferredGeometryRenderPass(const DeferredGeometryRenderPass&) = delete;
		DeferredGeometryRenderPass& operator=(const DeferredGeometryRenderPass&) = delete;

		// Movable:
		DeferredGeometryRenderPass(DeferredGeometryRenderPass&& other) noexcept = default;
		DeferredGeometryRenderPass& operator=(DeferredGeometryRenderPass&& other) noexcept = default;

		// Getters:
		GBufferTexture2d* GetAlbedoTexture(uint32_t frameIndex) const;
		GBufferTexture2d* GetNormalTexture(uint32_t frameIndex) const;
		GBufferTexture2d* GetMaterialTexture(uint32_t frameIndex) const;
		DepthTexture2d* GetDepthTexture(uint32_t frameIndex) const;

	private: // Methods:
		void CreateRenderPass();
		void CreateGBufferTextures(uint32_t renderWidth, uint32_t renderHeight);
		void CreateFrameBuffers();
	};
}