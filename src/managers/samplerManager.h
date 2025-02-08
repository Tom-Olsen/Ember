#ifndef __INCLUDE_GUARD_samplerManager_h__
#define __INCLUDE_GUARD_samplerManager_h__
#include <memory>
#include <unordered_map>
#include <string>



namespace emberEngine
{
    // Forward declarations:
    class Sampler;
    struct VulkanContext;



    /// <summary>
    /// Purely static class that takes care of lifetime of all Sampler objects.
    /// </summary>
    class SamplerManager
    {
    public: // Members

    private: // Members
        static bool s_isInitialized;
        static VulkanContext* s_pContext;
        static std::unordered_map<std::string, std::unique_ptr<Sampler>> s_samplers;

    public: // Methods
        static void Init(VulkanContext* pContext);
        static void Clear();

        static void AddSampler(Sampler* pSampler);
        static Sampler* GetSampler(const std::string& name);
        static void DeleteSampler(const std::string& name);

        static void PrintAllSamplerNames();

    private: // Methods
        // Delete all constructors:
        SamplerManager() = delete;
        SamplerManager(const SamplerManager&) = delete;
        SamplerManager& operator=(const SamplerManager&) = delete;
        ~SamplerManager() = delete;
    };
}



#endif // __INCLUDE_GUARD_samplerManager_h__