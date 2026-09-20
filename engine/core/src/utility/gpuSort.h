#pragma once
#include "emberCoreExport.h"
#include "bufferView.h"
#include "compute.h"
#include "computeShader.h"



namespace emberCore
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
        static ComputeShader s_localBitonicSortComputeShader;
        static ComputeShader s_bigFlipComputeShader;
        static ComputeShader s_bigDisperseComputeShader;
        static ComputeShader s_localDisperseComputeShader;
        // Bitonic permutation sort compute shaders:
        static ComputeShader s_localBitonicSortPermutationComputeShader;
        static ComputeShader s_bigFlipPermutationComputeShader;
        static ComputeShader s_bigDispersePermutationComputeShader;
        static ComputeShader s_localDispersePermutationComputeShader;
        // Other compute shaders:
        static ComputeShader s_initIndexBufferComputeShader;
        static ComputeShader s_applyPermutationComputeShader;
        static ComputeShader s_invertPermutationComputeShader;

    public: // Methods:
	    // Initialization/Cleanup:
        static void Init();
        static void Clear();

        // Dispatches:
        static void Sort(ComputeType computeType, BufferView<T>& bufferView, uint32_t sessionID = -1);
        static void SortPermutation(ComputeType computeType, BufferView<T>& bufferView, BufferView<uint32_t>& permutationBufferView, uint32_t sessionID = -1);
        static void ApplyPermutation(ComputeType computeType, BufferView<uint32_t>& permutationBufferView, BufferView<T>& inBufferView, BufferView<T>& outBufferView, uint32_t sessionID = -1);
        static void InvertPermutation(ComputeType computeType, BufferView<uint32_t>& permutationBufferView, BufferView<uint32_t>& inversePermutationBufferView, uint32_t sessionID = -1);

        // Dispatch and wait:
        static void SortAndWait(BufferView<T>& bufferView);
        static void SortPermutationAndWait(BufferView<T>& bufferView, BufferView<uint32_t>& permutationBufferView);
        static void ApplyPermutationAndWait(BufferView<uint32_t>& permutationBufferView, BufferView<T>& inBufferView, BufferView<T>& outBufferView);
        static void InvertPermutationAndWait(BufferView<uint32_t>& permutationBufferView, BufferView<uint32_t>& inversePermutationBufferView);

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