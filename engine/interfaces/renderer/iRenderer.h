#pragma once
#include "emberMath.h"
#include "commonLighting.h"



namespace emberBackendInterface
{
    // Forward declerations:
    class IBuffer;
    class ICompute;
    class IDearImGui;
    class IMaterial;
    class IMesh;
    class IShaderProperties;



    class IRenderer
    {
    public: // Methods:
        // Virtual destructor for v-table:
        virtual ~IRenderer() = default;

        // Main render loop:
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

        // Getters:
        virtual uint32_t GetShadowMapResolution() = 0;

        // Setters:
        virtual void SetIComputeHandle(emberBackendInterface::ICompute* pICompute) = 0;
        virtual void SetIDearImGuiHandle(emberBackendInterface::IDearImGui* pIDearImGui) = 0;
        virtual void SetActiveCamera(const Float3& position, const Float4x4& viewMatrix, const Float4x4& projectionMatrix) = 0;

        // Functionallity forwarding:
        virtual void CollectGarbage() = 0;
        virtual void WaitDeviceIdle() = 0;

        // Vulkan handle passthrough for API coupling:
        virtual void* GetVkInstance() = 0;
        virtual void* GetVkPhysicalDevice() = 0;
        virtual void* GetVkDevice() = 0;
        virtual void* GetPresentVkRenderPass() = 0;
        virtual void* GetVkDescriptorPool() = 0;
        virtual void* GetGraphicsVkQueue() = 0;
        virtual void* GetColorSampler() = 0;
        virtual uint32_t GetGraphicsVkQueueFamilyIndex() = 0;
        virtual uint32_t GetSwapchainImageCount() = 0;
        virtual uint32_t GetFramesInFlight() = 0;
    };
}