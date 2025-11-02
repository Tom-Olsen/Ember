#pragma once
#include "emberEngine.h"
using namespace emberEngine;



namespace fluidDynamics
{
    class SphBitonicSort2d
    {
    private: // Members
        ComputeShader m_cellKeys;
        ComputeShader m_startIndices;
        ComputeShader m_localBitonicSort;
        ComputeShader m_bigFlip;
        ComputeShader m_bigDisperse;
        ComputeShader m_localDisperse;
        ShaderProperties m_cellKeyProperties;
        ShaderProperties m_startIndicesProperties;

    public: // Methods
        SphBitonicSort2d();
        ~SphBitonicSort2d();

        void ComputeCellKeys(Buffer& cellKeyBuffer, Buffer& positionBuffer, float gridRadius);
        void ComputeStartIndices(Buffer& cellKeyBuffer, Buffer& startIndexBuffer);
        void Sort(Buffer& cellKeyBuffer, Buffer& positionBuffer, Buffer& velocityBuffer);
    };
}