#pragma once
#include "commonMaterialRenderState.h"
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
        virtual void SetDefaultRenderState(const emberCommon::MaterialRenderState& defaultRenderState) = 0;
        virtual void SetShadowMaterial(IMaterial* pShadowMaterial) = 0;

        // Getters:
        virtual const std::string& GetName() const = 0;
        virtual const emberCommon::MaterialRenderState& GetDefaultRenderState() const = 0;
        virtual IMaterial* GetShadowMaterial() const = 0;
        virtual IDescriptorSetBinding* GetShaderDescriptorSetBinding() const = 0;

        // Debugging:
        virtual void Print() const = 0;
    };
}
