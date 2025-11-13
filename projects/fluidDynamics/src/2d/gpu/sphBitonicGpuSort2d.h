#pragma once
#include "emberEngine.h"
using namespace emberEngine;



namespace fluidDynamics
{
    class SphBitonicGpuSort2d
    {
    private: // Members
        int m_hashGridSize;
        float m_gridRadius;
        ComputeShader m_cellKeys;
        ComputeShader m_startIndices;
        ComputeShader m_localBitonicSort;
        ComputeShader m_bigFlip;
        ComputeShader m_bigDisperse;
        ComputeShader m_localDisperse;
        ShaderProperties m_cellKeyProperties;
        ShaderProperties m_startIndicesProperties;

    public: // Methods
        SphBitonicGpuSort2d();
        ~SphBitonicGpuSort2d();

        // Setters:
        void SetHashGridSize(int particleCount);
        void SetGridRadius(float gridRadius);

        // Sorting:
        void ComputeCellKeys(Buffer& cellKeyBuffer, Buffer& positionBuffer);
        void ComputeStartIndices(Buffer& cellKeyBuffer, Buffer& startIndexBuffer);
        void Sort(Buffer& cellKeyBuffer, Buffer& positionBuffer, Buffer& velocityBuffer);

        // TODO:
        // -implement the following
        // -remove above sort.
        // -remove ComputeCellKeys and ComputeStartIndices from this class => this class is for sorting on gpu only!
        Buffer m_permutationBuffer; // resize if needed
        void ComputeSortPermutation(Buffer& cellKeyBuffer);
        void ApplySortPermutation(Buffer& cellKeyBuffer, Buffer& buffer);
    };
}