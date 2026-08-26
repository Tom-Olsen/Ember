#pragma once
#include <cstdint>
#include <memory>
#include <vector>



namespace vulkanRendererBackend
{
    class DeferredGeometryRenderPass;
    class DeferredLightingRenderPass;
    class DepthTexture2d;
    class ShadowRenderPass;
    class ForwardRenderPass;
    class GizmoRenderPass;
    class OutlineRenderPass;
    class PresentRenderPass;
    class RenderTexture2d;



    /// <summary>
    /// Purely static class that takes care of lifetime of all RenderPass objects.
    /// </summary>
    class RenderPassManager
    {
    private: // Members
        static bool s_isInitialized;
        static std::unique_ptr<GizmoRenderPass> s_pGizmoRenderPass;
        static std::unique_ptr<OutlineRenderPass> s_pOutlineRenderPass;
        static std::unique_ptr<ShadowRenderPass> s_pShadowRenderPass;
        static std::unique_ptr<DeferredGeometryRenderPass> s_pDeferredGeometryRenderPass;
        static std::unique_ptr<DeferredLightingRenderPass> s_pDeferredLightingRenderPass;
        static std::unique_ptr<ForwardRenderPass> s_pForwardRenderPass;
        static std::unique_ptr<PresentRenderPass> s_pPresentRenderPass;

    public: // Methods
	    // Initialization/Cleanup:
        static void Init(
            uint32_t renderWidth,
            uint32_t renderHeight,
            uint32_t shadowMapResolution,
            uint32_t maxLightsCount,
            const std::vector<std::unique_ptr<RenderTexture2d>>& pSceneColorTextures,
            const std::vector<std::unique_ptr<RenderTexture2d>>& pSecondarySceneColorTextures,
            const std::vector<std::unique_ptr<DepthTexture2d>>& pSceneDepthTextures);
        static void Clear();
        static void RecreateRenderPasses();

        // Getters:
        static GizmoRenderPass* GetGizmoRenderPass();
        static OutlineRenderPass* GetOutlineRenderPass();
        static ShadowRenderPass* GetShadowRenderPass();
        static DeferredGeometryRenderPass* GetDeferredGeometryRenderPass();
        static DeferredLightingRenderPass* GetDeferredLightingRenderPass();
        static ForwardRenderPass* GetForwardRenderPass();
        static PresentRenderPass* GetPresentRenderPass();

    private: // Methods
        // Delete all constructors:
        RenderPassManager() = delete;
        RenderPassManager(const RenderPassManager&) = delete;
        RenderPassManager& operator=(const RenderPassManager&) = delete;
        RenderPassManager(RenderPassManager&&) = delete;
        RenderPassManager& operator=(RenderPassManager&&) = delete;
        ~RenderPassManager() = delete;
    };
}