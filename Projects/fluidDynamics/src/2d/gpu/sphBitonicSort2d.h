#ifndef __INCLUDE_GUARD_sphBitonicSort2d_h__
#define __INCLUDE_GUARD_sphBitonicSort2d_h__
#include <memory>



namespace emberEngine
{
    // Forward declerations:
    class ComputeShader;
    class StorageBuffer;
    class ShaderProperties;



    class SphBitonicSort2d
    {
    private: // Members
        std::unique_ptr<ComputeShader> m_pCellKeys;
        std::unique_ptr<ComputeShader> m_pStartIndices;
        std::unique_ptr<ComputeShader> m_pLocalBitonicSort;
        std::unique_ptr<ComputeShader> m_pBigFlip;
        std::unique_ptr<ComputeShader> m_pBigDisperse;
        std::unique_ptr<ComputeShader> m_pLocalDisperse;
        std::unique_ptr<ShaderProperties> m_pCellKeyProperties;
        std::unique_ptr<ShaderProperties> m_pStartIndicesProperties;

    public: // Methods
        SphBitonicSort2d();
        ~SphBitonicSort2d();

        void ComputeCellKeys(StorageBuffer* pCellKeyBuffer, StorageBuffer* pPositionBuffer, float gridRadius);
        void ComputeStartIndices(StorageBuffer* pCellKeyBuffer, StorageBuffer* pStartIndexBuffer);
        void Sort(StorageBuffer* pCellKeyBuffer, StorageBuffer* pPositionBuffer, StorageBuffer* pVelocityBuffer);
    };
}



#endif // __INCLUDE_GUARD_sphBitonicSort2d_h__