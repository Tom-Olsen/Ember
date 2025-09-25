#pragma once
#include "emberMath.h"
#include "commonLighting.h"



// Forward declerations:
namespace vulkanRendererBackend
{
    class Material;
    class Mesh;
    class ShaderProperties;
    class StorageBuffer;
}



namespace emberBackendInterface
{
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
        virtual void DrawMesh(vulkanRendererBackend::Mesh* pMesh, vulkanRendererBackend::Material* pMaterial, vulkanRendererBackend::ShaderProperties* pShaderProperties, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows) = 0;
        virtual vulkanRendererBackend::ShaderProperties* DrawMesh(vulkanRendererBackend::Mesh* pMesh, vulkanRendererBackend::Material* pMaterial, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows) = 0;

        // Draw instanced:
        virtual void DrawInstanced(uint32_t instanceCount, vulkanRendererBackend::StorageBuffer* pInstanceBuffer, vulkanRendererBackend::Mesh* pMesh, vulkanRendererBackend::Material* pMaterial, vulkanRendererBackend::ShaderProperties* pShaderProperties, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows) = 0;
        virtual vulkanRendererBackend::ShaderProperties* DrawInstanced(uint32_t instanceCount, vulkanRendererBackend::StorageBuffer* pInstanceBuffer, vulkanRendererBackend::Mesh* pMesh, vulkanRendererBackend::Material* pMaterial, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows) = 0;
    };
}