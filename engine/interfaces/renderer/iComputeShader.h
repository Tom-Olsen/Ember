#pragma once
#include "emberMath.h"
#include <string>



namespace emberBackendInterface
{
    class IDescriptorSetBinding;



    class IComputeShader
    {
    public: // Methods:
        // Virtual destructor for v-table:
        virtual ~IComputeShader() = default;

        // Getters:
        virtual const std::string& GetName() const = 0;
        virtual Uint3 GetBlockSize() const = 0;
        virtual IDescriptorSetBinding* GetShaderDescriptorSetBinding() const = 0;

        // Debugging:
        virtual void Print() const = 0;
    };
}