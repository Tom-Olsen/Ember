#ifndef __INCLUDE_GUARD_bitonicSort_h__
#define __INCLUDE_GUARD_bitonicSort_h__
#include <memory>



namespace emberEngine
{
    // Forward declerations:
    class ComputeShader;
    class StorageBuffer;



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
        static void Sort(StorageBuffer* buffer, int count);

    private: // Methods
        // Delete all constructors:
        BitonicSort() = delete;
        BitonicSort(const BitonicSort&) = delete;
        BitonicSort& operator=(const BitonicSort&) = delete;
        ~BitonicSort() = delete;
    };
}



#endif // __INCLUDE_GUARD_bitonicSort_h__