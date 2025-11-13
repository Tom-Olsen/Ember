#pragma once
#include "emberCoreExport.h"
#include "emberEngine.h"



namespace emberEngine
{
    /// <summary>
    /// Sorts given BufferView in ascending order using bitonic sort.
    /// Sort/ApplyPermutation customization requires custom shaders: localBitonicSort<T>, bigFlip<T>, bigDisperse<T>, localDisperse<T>, applyPermutation<T>.
    /// Simply copy paste existing compute shaders and modify dataBuffer type and CompareAndSwap function to your liking.
    /// </summary>
    template <typename T>
    class EMBER_CORE_API GpuSort
    {
    private: // Members
        static bool s_isInitialized;
        // Bitonic sort compute shaders:
        static std::unique_ptr<ComputeShader> s_pLocalBitonicSortComputeShader;
        static std::unique_ptr<ComputeShader> s_pBigFlipComputeShader;
        static std::unique_ptr<ComputeShader> s_pBigDisperseComputeShader;
        static std::unique_ptr<ComputeShader> s_pLocalDisperseComputeShader;
        // Apply permutation resources:
        static std::unique_ptr<ComputeShader> s_pApplyPermutationComputeShader;
        static std::unique_ptr<ShaderProperties> s_pApplyPermutationShaderProperties;

    public: // Methods:
	    // Initialization/Cleanup:
        static void Init();
        static void Clear();

        // Sort:
        static void Sort(ComputeType computeType, BufferView<T>& buffer);

        // Permutation sort:
        static void SortPermutation(ComputeType computeType, BufferView<T>& buffer, Buffer& permutationBuffer);

        // Apply permutation:
        static void ApplyPermutation(ComputeType computeType, Buffer& permutationBuffer, BufferView<T>& inBuffer, BufferView<T>& outBuffer);

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