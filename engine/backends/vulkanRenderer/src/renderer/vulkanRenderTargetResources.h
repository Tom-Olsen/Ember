#pragma once
#include <cstdint>
#include <memory>
#include <vector>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class DepthTexture2d;
	class DepthTexture2dArray;
	class GBufferTexture2d;
	class RenderTexture2d;
	class SceneColorTexture2dPair;
	class StorageTexture2d;
	class VmaImage;



	/// <summary>
	/// Owns offscreen framebuffer attachments and render targets used by render passes, stages, compute, and descriptor sets.
	/// </summary>
	class RenderTargetResources
	{
	private: // Members:
		std::unique_ptr<SceneColorTexture2dPair> m_pSceneColorTexturePair;
		std::vector<std::unique_ptr<DepthTexture2d>> m_pSceneDepthTextures;
		std::vector<std::unique_ptr<GBufferTexture2d>> m_pAlbedoTextures;
		std::vector<std::unique_ptr<GBufferTexture2d>> m_pNormalTextures;
		std::vector<std::unique_ptr<GBufferTexture2d>> m_pSurfacePropertiesTextures;
		std::vector<std::unique_ptr<RenderTexture2d>> m_pOutlineTextures;
		std::vector<std::unique_ptr<RenderTexture2d>> m_pGizmoTextures;
		std::vector<std::unique_ptr<VmaImage>> m_pGizmoMsaaImages;
		std::vector<std::unique_ptr<VmaImage>> m_pGizmoDepthImages;
		std::vector<std::unique_ptr<StorageTexture2d>> m_pExpandedOutlineMaskTextures;
		std::vector<std::unique_ptr<StorageTexture2d>> m_pHorizontalExpandedOutlineMaskTextures;
		std::unique_ptr<DepthTexture2dArray> m_pShadowMaps;
		uint32_t m_shadowMapResolution;
		uint32_t m_shadowMapLayerCount;

	public: // Methods:
		RenderTargetResources(uint32_t renderWidth, uint32_t renderHeight, uint32_t shadowMapResolution, uint32_t maxLightsCount);
		~RenderTargetResources();

		// Non-copyable:
		RenderTargetResources(const RenderTargetResources&) = delete;
		RenderTargetResources& operator=(const RenderTargetResources&) = delete;

		// Movable:
		RenderTargetResources(RenderTargetResources&& other) noexcept;
		RenderTargetResources& operator=(RenderTargetResources&& other) noexcept;

		// Getters:
		uint32_t GetFrameCount() const;
		uint32_t GetShadowMapResolution() const;
		uint32_t GetShadowMapLayerCount() const;
		SceneColorTexture2dPair& GetSceneColorTexturePair();
		const SceneColorTexture2dPair& GetSceneColorTexturePair() const;
		DepthTexture2d& GetSceneDepthTexture(uint32_t frameIndex);
		const DepthTexture2d& GetSceneDepthTexture(uint32_t frameIndex) const;
		GBufferTexture2d& GetAlbedoTexture(uint32_t frameIndex);
		const GBufferTexture2d& GetAlbedoTexture(uint32_t frameIndex) const;
		GBufferTexture2d& GetNormalTexture(uint32_t frameIndex);
		const GBufferTexture2d& GetNormalTexture(uint32_t frameIndex) const;
		GBufferTexture2d& GetSurfacePropertiesTexture(uint32_t frameIndex);
		const GBufferTexture2d& GetSurfacePropertiesTexture(uint32_t frameIndex) const;
		RenderTexture2d& GetOutlineTexture(uint32_t frameIndex);
		const RenderTexture2d& GetOutlineTexture(uint32_t frameIndex) const;
		RenderTexture2d& GetGizmoTexture(uint32_t frameIndex);
		const RenderTexture2d& GetGizmoTexture(uint32_t frameIndex) const;
		VmaImage& GetGizmoMsaaImage(uint32_t frameIndex);
		const VmaImage& GetGizmoMsaaImage(uint32_t frameIndex) const;
		VmaImage& GetGizmoDepthImage(uint32_t frameIndex);
		const VmaImage& GetGizmoDepthImage(uint32_t frameIndex) const;
		StorageTexture2d& GetExpandedOutlineMaskTexture(uint32_t frameIndex);
		const StorageTexture2d& GetExpandedOutlineMaskTexture(uint32_t frameIndex) const;
		StorageTexture2d& GetHorizontalExpandedOutlineMaskTexture(uint32_t frameIndex);
		const StorageTexture2d& GetHorizontalExpandedOutlineMaskTexture(uint32_t frameIndex) const;
		DepthTexture2dArray& GetShadowMaps();
		const DepthTexture2dArray& GetShadowMaps() const;

	private: // Methods:
		void CreateGizmoMsaaImages();
		void CreateGizmoDepthImages();
	};
}