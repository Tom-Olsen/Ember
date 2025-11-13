#pragma once
#include "emberCoreExport.h"
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
    class EMBER_CORE_API ComputeShaderManager
    {
    private: // Members
        static bool s_isInitialized;
        static std::unordered_map<std::string, std::unique_ptr<ComputeShader>> s_computeShaders;

    public: // Methods
        // Initialization/Cleanup:
        static void Init();
        static void Clear();

        static void AddComputeShader(ComputeShader&& computeShader);  // must be called as AddComputeShader(std::move(computeShader)). Leaves input computeShader empty.
        static ComputeShader& GetComputeShader(const std::string& name);
        static ComputeShader* TryGetComputeShader(const std::string& name);
        static void DeleteComputeShader(const std::string& name);

        static void Print();

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