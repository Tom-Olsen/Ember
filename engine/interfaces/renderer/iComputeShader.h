#pragma once
#include "emberMath.h"



namespace emberBackendInterface
{
    class IComputeShader
    {
    public: // Methods:
        // Virtual destructor for v-table:
        virtual ~IComputeShader() = default;

        // Getters:
        virtual Uint3 GetBlockSize() const = 0;
    };
}