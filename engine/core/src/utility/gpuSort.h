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
        // Bitonic sort compute shaders:
        static std::unique_ptr<ComputeShader> s_pLocalBitonicSortIntComputeShader;
        static std::unique_ptr<ComputeShader> s_pBigFlipIntComputeShader;
        static std::unique_ptr<ComputeShader> s_pBigDisperseIntComputeShader;
        static std::unique_ptr<ComputeShader> s_pLocalDisperseIntComputeShader;
        // Apply permutation compute shaders:
        static std::unique_ptr<ComputeShader> s_pApplyPermutationFloat1ComputeShader;
        static std::unique_ptr<ComputeShader> s_pApplyPermutationFloat2ComputeShader;
        static std::unique_ptr<ComputeShader> s_pApplyPermutationFloat3ComputeShader;
        static std::unique_ptr<ComputeShader> s_pApplyPermutationFloat4ComputeShader;
        // Apply permutation shader properties:
        static std::unique_ptr<ShaderProperties> s_pApplyPermutationFloat1ShaderProperties;
        static std::unique_ptr<ShaderProperties> s_pApplyPermutationFloat2ShaderProperties;
        static std::unique_ptr<ShaderProperties> s_pApplyPermutationFloat3ShaderProperties;
        static std::unique_ptr<ShaderProperties> s_pApplyPermutationFloat4ShaderProperties;

    public: // Methods:
	    // Initialization/Cleanup:
        static void Init();
        static void Clear();

        // ToDo: add BufferView<T> class and use it for proper branching of below methods for different(all) emberMath types.

        // Sort:
        static void Sort(ComputeType computeType, Buffer& buffer);

        // Permutation sort:
        static void SortPermutation(ComputeType computeType, Buffer& buffer, Buffer& permutationBuffer);

        // Apply permutation:
        static void ApplyPermutation(ComputeType computeType, Buffer& permutationBuffer, Buffer& inBuffer, Buffer& outBuffer);

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