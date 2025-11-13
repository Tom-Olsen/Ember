#pragma once
#include "emberCoreExport.h"
#include "emberEngine.h"



namespace emberEngine
{
    /// <summary>
    /// Sorts given Buffer in ascending order using bitonic sort.
    /// Ember::ToDo: split Sort in multiple versions, SortInt(...), SortFloat(...), etc.
    /// Sort customization requires custom shaders: localBitonicSort, bigFlip, bigDisperse, localDisperse.
    /// Simply copy paste existing compute shaders and modify dataBuffer type and CompareAndSwap function to your liking.
    /// </summary>
    class EMBER_CORE_API GpuSort
    {
    private: // Members
        static bool s_isInitialized;
        static std::unique_ptr<ComputeShader> s_pLocalBitonicSortComputeShader;
        static std::unique_ptr<ComputeShader> s_pBigFlipComputeShader;
        static std::unique_ptr<ComputeShader> s_pBigDisperseComputeShader;
        static std::unique_ptr<ComputeShader> s_pLocalDisperseComputeShader;
        static std::unique_ptr<ComputeShader> s_pReorderFloat1ComputeShader;
        static std::unique_ptr<ComputeShader> s_pReorderFloat2ComputeShader;
        static std::unique_ptr<ComputeShader> s_pReorderFloat3ComputeShader;
        static std::unique_ptr<ComputeShader> s_pReorderFloat4ComputeShader;
        static std::unique_ptr<ShaderProperties> s_pReorderFloat1ShaderProperties;
        static std::unique_ptr<ShaderProperties> s_pReorderFloat2ShaderProperties;
        static std::unique_ptr<ShaderProperties> s_pReorderFloat3ShaderProperties;
        static std::unique_ptr<ShaderProperties> s_pReorderFloat4ShaderProperties;

    public: // Methods:
	    // Initialization/Cleanup:
        static void Init();
        static void Clear();

        // Sorting:
        static void SortImmediate(Buffer& buffer);
        static void SortAsync(uint32_t sessionID, Buffer& buffer);
        static void SortPreRender(Buffer& buffer);

        // Reorder immediate:
        static void ReorderFloatImmediate(Buffer& indexBuffer, Buffer& inBuffer, Buffer& outBuffer);
        static void ReorderFloat2Immediate(Buffer& indexBuffer, Buffer& inBuffer, Buffer& outBuffer);
        static void ReorderFloat3Immediate(Buffer& indexBuffer, Buffer& inBuffer, Buffer& outBuffer);
        static void ReorderFloat4Immediate(Buffer& indexBuffer, Buffer& inBuffer, Buffer& outBuffer);

        // Reorder async:
        static void ReorderFloatAsync(uint32_t sessionID, Buffer& indexBuffer, Buffer& inBuffer, Buffer& outBuffer);
        static void ReorderFloat2Async(uint32_t sessionID, Buffer& indexBuffer, Buffer& inBuffer, Buffer& outBuffer);
        static void ReorderFloat3Async(uint32_t sessionID, Buffer& indexBuffer, Buffer& inBuffer, Buffer& outBuffer);
        static void ReorderFloat4Async(uint32_t sessionID, Buffer& indexBuffer, Buffer& inBuffer, Buffer& outBuffer);

        // Reorder pre render:
        static void ReorderFloatPreRender(Buffer& indexBuffer, Buffer& inBuffer, Buffer& outBuffer);
        static void ReorderFloat2PreRender(Buffer& indexBuffer, Buffer& inBuffer, Buffer& outBuffer);
        static void ReorderFloat3PreRender(Buffer& indexBuffer, Buffer& inBuffer, Buffer& outBuffer);
        static void ReorderFloat4PreRender(Buffer& indexBuffer, Buffer& inBuffer, Buffer& outBuffer);

    private: // Methods
        // Delete all constructors:
        GpuSort() = delete;
        GpuSort(const GpuSort&) = delete;
        GpuSort& operator=(const GpuSort&) = delete;
        GpuSort(GpuSort&&) = delete;
        GpuSort& operator=(GpuSort&&) = delete;
        ~GpuSort() = delete;
    };
}