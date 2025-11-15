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

        static std::unique_ptr<ComputeShader> s_pLocalBitonicSortComputeShader;
        static std::unique_ptr<ComputeShader> s_pBigFlipComputeShader;
        static std::unique_ptr<ComputeShader> s_pBigDisperseComputeShader;
        static std::unique_ptr<ComputeShader> s_pLocalDisperseComputeShader;

        static std::unique_ptr<ComputeShader> s_pLocalBitonicSortPermutationComputeShader;
        static std::unique_ptr<ComputeShader> s_pBigFlipPermutationComputeShader;
        static std::unique_ptr<ComputeShader> s_pBigDispersePermutationComputeShader;
        static std::unique_ptr<ComputeShader> s_pLocalDispersePermutationComputeShader;

        static std::unique_ptr<ComputeShader> s_pApplyPermutationComputeShader;
        static std::unique_ptr<ShaderProperties> s_pApplyPermutationShaderProperties;

    public: // Methods:
	    // Initialization/Cleanup:
        static void Init();
        static void Clear();

        // Sort:
        static void Sort(ComputeType computeType, BufferView<T>& bufferView);

        // Permutation sort:
        static void SortPermutation(ComputeType computeType, BufferView<T>& bufferView, BufferView<int>& permutationBufferView);

        // Apply permutation:
        static void ApplyPermutation(ComputeType computeType, BufferView<int>& permutationBufferView, BufferView<T>& inBufferView, BufferView<T>& outBufferView, uint32_t sessionID = -1);

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