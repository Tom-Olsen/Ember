#ifndef __INCLUDE_GUARD_renderPassManager_h__
#define __INCLUDE_GUARD_renderPassManager_h__
#include <memory>
#include <string>
#include <unordered_map>



struct VulkanContext;
class RenderPass;



/// <summary>
/// Purely static class that takes care of lifetime of all RenderPasses objects.
/// </summary>
class RenderPassManager
{
public: // Members

private: // Members
    static bool isInitialized;
    static VulkanContext* context;
    static std::unordered_map<std::string, std::unique_ptr<RenderPass>> renderPasses;

public: // Methods
    static void Init(VulkanContext* vulkanContext);
    static void Clear();
	static void RecreateRenderPasses();

    static void AddRenderPass(const std::string name, RenderPass* renderPass);
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



#endif // __INCLUDE_GUARD_renderPassManager_h__