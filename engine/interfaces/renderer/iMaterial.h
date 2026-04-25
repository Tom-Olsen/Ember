#pragma once
#include "commonPipelineState.h"
#include <string>



namespace emberBackendInterface
{
    class IDescriptorSetBinding;



    class IMaterial
    {
    public: // Methods:
        // Virtual destructor for v-table:
        virtual ~IMaterial() = default;

        // Setters:
        virtual void SetRenderQueue(uint32_t renderQueue) = 0;
        virtual void SetRenderMode(emberCommon::RenderMode renderMode) = 0;

        // Getters:
        virtual const std::string& GetName() const = 0;
        virtual uint32_t GetRenderQueue() const = 0;
        virtual emberCommon::RenderMode GetRenderMode() const = 0;
        virtual IDescriptorSetBinding* GetShaderDescriptorSetBinding() const = 0;

        // Debugging:
        virtual void Print() const = 0;
    };
}