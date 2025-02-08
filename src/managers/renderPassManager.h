#ifndef __INCLUDE_GUARD_renderPassManager_h__
#define __INCLUDE_GUARD_renderPassManager_h__
#include <memory>
#include <string>
#include <unordered_map>



namespace emberEngine
{
    // Forward declarations:
    struct VulkanContext;
    class RenderPass;



    /// <summary>
    /// Purely static class that takes care of lifetime of all RenderPass objects.
    /// </summary>
    class RenderPassManager
    {
    private: // Members
        static bool s_isInitialized;
        static VulkanContext* s_pContext;
        static std::unordered_map<std::string, std::unique_ptr<RenderPass>> s_renderPasses;

    public: // Methods
        static void Init(VulkanContext* pContext);
        static void Clear();
        static void RecreateRenderPasses();

        static void AddRenderPass(const std::string name, RenderPass* pRenderPass);
        static RenderPass* GetRenderPass(const std::string& name);
        static void DeleteRenderPass(const std::string& name);

        static void PrintAllRenderPassNames();

    private: // Methods
        // Delete all constructors:
        RenderPassManager() = delete;
        RenderPassManager(const RenderPassManager&) = delete;
        RenderPassManager& operator=(const RenderPassManager&) = delete;
        ~RenderPassManager() = delete;
    };
}



#endif // __INCLUDE_GUARD_renderPassManager_h__