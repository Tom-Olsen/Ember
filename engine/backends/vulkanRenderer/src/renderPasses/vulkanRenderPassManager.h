#pragma once
#include <cstdint>
#include <memory>



namespace vulkanRendererBackend
{
    class ShadowRenderPass;
    class ForwardRenderPass;
    class GizmoRenderPass;
    class OutlineRenderPass;
    class PresentRenderPass;



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
        static std::unique_ptr<ForwardRenderPass> s_pForwardRenderPass;
        static std::unique_ptr<PresentRenderPass> s_pPresentRenderPass;

    public: // Methods
	    // Initialization/Cleanup:
        static void Init(uint32_t renderWidth, uint32_t renderHeight, uint32_t shadowMapResolution, uint32_t maxLightsCount);
        static void Clear();
        static void RecreateRenderPasses();

        // Getters:
        static GizmoRenderPass* GetGizmoRenderPass();
        static OutlineRenderPass* GetOutlineRenderPass();
        static ShadowRenderPass* GetShadowRenderPass();
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