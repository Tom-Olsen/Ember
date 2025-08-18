#ifndef __INCLUDE_GUARD_computeShaderManager_h__
#define __INCLUDE_GUARD_computeShaderManager_h__
#include <unordered_map>
#include <memory>
#include <string>



namespace emberEngine
{
    // Forward declarations:
    class ComputeShader;



    /// <summary>
    /// Purely static class that takes care of lifetime of all Material objects.
    /// </summary>
    class ComputeShaderManager
    {
    private: // Members
        static bool s_isInitialized;
        static std::unordered_map<std::string, std::unique_ptr<ComputeShader>> s_computeShaders;

    public: // Methods
        static void Init();
        static void Clear();

        static void AddComputeShader(ComputeShader* pComputeShader);
        static ComputeShader* GetComputeShader(const std::string& name);
        static void DeleteComputeShader(const std::string& name);

        static void PrintAllComputeShaderNames();

    private: // Methods
        // Delete all constructors:
        ComputeShaderManager() = delete;
        ComputeShaderManager(const ComputeShaderManager&) = delete;
        ComputeShaderManager& operator=(const ComputeShaderManager&) = delete;
        ComputeShaderManager(ComputeShaderManager&&) = delete;
        ComputeShaderManager& operator=(ComputeShaderManager&&) = delete;
        ~ComputeShaderManager() = delete;
    };
}



#endif // __INCLUDE_GUARD_computeShaderManager_h__