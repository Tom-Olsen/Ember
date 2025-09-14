#pragma once
#include "emberMath.h"



namespace emberBackendInterface
{
    class IComputeShader
    {
    public: // Virtual destructor for v-table:
        virtual ~IComputeShader() = default;

    public: // Methods:
        virtual Uint3 GetBlockSize() const = 0;
    };
}