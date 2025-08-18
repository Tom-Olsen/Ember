#ifndef __INCLUDE_GUARD_bitonicSort_h__
#define __INCLUDE_GUARD_bitonicSort_h__
#include <memory>



namespace emberEngine
{
    // Forward declerations:
    class ComputeShader;
    class StorageBuffer;


    /// <summary>
    /// Sorts given int StorageBuffer in ascending order.
    /// Sort customization requires custom shaders: localBitonicSort, bigFlip, bigDisperse, localDisperse.
    /// Simpy copy paste existing compute shaders and modify dataBuffer type and CompareAndSwap function to your liking.
    /// Ideally you create a new class for the your sorting algorithm, identically to this class.
    /// </summary>
    class BitonicSort
    {
    private: // Members
        static bool s_isInitialized;
        static std::unique_ptr<ComputeShader> s_pLocalBitonicSort;
        static std::unique_ptr<ComputeShader> s_pBigFlip;
        static std::unique_ptr<ComputeShader> s_pBigDisperse;
        static std::unique_ptr<ComputeShader> s_pLocalDisperse;

    public: // Methods
        static void Init();
        static void Clear();

        // Sorting:
        static void Sort(StorageBuffer* buffer);

    private: // Methods
        // Delete all constructors:
        BitonicSort() = delete;
        BitonicSort(const BitonicSort&) = delete;
        BitonicSort& operator=(const BitonicSort&) = delete;
        ~BitonicSort() = delete;
    };
}



#endif // __INCLUDE_GUARD_bitonicSort_h__