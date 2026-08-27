#pragma once
#include "commonCullMode.h"
#include "commonDeferredGeometryRenderState.h"
#include "commonDeferredLightingRenderState.h"
#include "commonForwardRenderState.h"
#include "commonGizmoRenderState.h"
#include "commonMaterialPass.h"
#include <cstdint>



namespace emberBackendInterface
{
    class IDescriptorSetBinding;



    class IMaterial
    {
    public: // Methods:
        // Virtual destructor for v-table:
        virtual ~IMaterial() = default;

        // Setters:
        virtual void SetRenderQueue(int32_t renderQueue) = 0;
        virtual void SetCullMode(emberCommon::CullMode cullMode) = 0;
        virtual void SetForwardRenderMode(emberCommon::ForwardRenderMode renderMode) = 0;
        virtual void SetGizmoRenderMode(emberCommon::GizmoRenderMode renderMode) = 0;

        // Getters:
        virtual emberCommon::MaterialPass GetMaterialPass() const = 0;
        virtual IDescriptorSetBinding* GetShaderDescriptorSetBinding() const = 0;
        virtual int32_t GetRenderQueue() const = 0;
        virtual emberCommon::CullMode GetCullMode() const = 0;
        virtual bool IsTransparent() const = 0;
        virtual emberCommon::GizmoRenderMode GetGizmoRenderMode() const = 0;
        virtual emberCommon::ForwardRenderMode GetForwardRenderMode() const = 0;
        virtual const emberCommon::GizmoRenderState& GetGizmoRenderState() const = 0;
        virtual const emberCommon::DeferredGeometryRenderState& GetDeferredGeometryRenderState() const = 0;
        virtual const emberCommon::DeferredLightingRenderState& GetDeferredLightingRenderState() const = 0;
        virtual const emberCommon::ForwardRenderState& GetForwardRenderState() const = 0;

        // Debugging:
        virtual void Print() const = 0;
    };
}