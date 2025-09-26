#pragma once
#include "emberMath.h"
#include "commonLighting.h"



namespace emberBackendInterface
{
    // Forward declerations:
    class IBuffer;
    class IMaterial;
    class IMesh;
    class IShaderProperties;



    class IRenderer
    {
    public: // Methods:
        // Virtual destructor for v-table:
        virtual ~IRenderer() = default;

        // Main render call:
        virtual void RenderFrame(int windowWidth, int windowHeight, float time, float deltaTime) = 0;

        // Lightsources:
        virtual void AddDirectionalLight(const Float3& direction, float intensity, const Float3& color, emberCommon::ShadowType shadowType, const Float4x4& worldToClipMatrix) = 0;
        virtual void AddPositionalLight(const Float3& position, float intensity, const Float3& color, emberCommon::ShadowType shadowType, float blendStart, float blendEnd, const Float4x4& worldToClipMatrix) = 0;

        // Draw mesh:
        virtual void DrawMesh(IMesh* pMesh, IMaterial* pMaterial, IShaderProperties* pShaderProperties, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows) = 0;
        virtual IShaderProperties* DrawMesh(IMesh* pMesh, IMaterial* pMaterial, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows) = 0;

        // Draw instanced:
        virtual void DrawInstanced(uint32_t instanceCount, IBuffer* pInstanceBuffer, IMesh* pMesh, IMaterial* pMaterial, IShaderProperties* pShaderProperties, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows) = 0;
        virtual IShaderProperties* DrawInstanced(uint32_t instanceCount, IBuffer* pInstanceBuffer, IMesh* pMesh, IMaterial* pMaterial, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows) = 0;
    };
}