#ifndef __INCLUDE_GUARD_renderPassManager_h__
#define __INCLUDE_GUARD_renderPassManager_h__
#include <memory>
#include <string>
#include <unordered_map>



namespace emberEngine
{
    // Forward declarations:
    namespace vulkanBackend
    {
        class ShadowRenderPass;
        class ForwardRenderPass;
        class PresentRenderPass;
    }



    /// <summary>
    /// Purely static class that takes care of lifetime of all RenderPass objects.
    /// </summary>
    class RenderPassManager
    {
    private: // Members
        static bool s_isInitialized;
        static uint32_t s_renderWidth;
        static uint32_t s_renderHeight;
        static std::unique_ptr<vulkanBackend::ShadowRenderPass> s_pShadowRenderPass;
        static std::unique_ptr<vulkanBackend::ForwardRenderPass> s_pForwardRenderPass;
        static std::unique_ptr<vulkanBackend::PresentRenderPass> s_pPresentRenderPass;

    public: // Methods
        static void Init(uint32_t renderWidth, uint32_t renderHeight);
        static void Clear();
        static void RecreateRenderPasses();

        // Getters:
        static vulkanBackend::ShadowRenderPass* GetShadowRenderPass();
        static vulkanBackend::ForwardRenderPass* GetForwardRenderPass();
        static vulkanBackend::PresentRenderPass* GetPresentRenderPass();

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



#endif // __INCLUDE_GUARD_renderPassManager_h__