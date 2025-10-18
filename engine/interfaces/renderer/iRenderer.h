#pragma once
#include "emberMath.h"
#include "commonBufferUsage.h"
#include "commonLighting.h"
#include "commonMaterialType.h"
#include "commonTextureFormat.h"
#include "commonTextureUsage.h"
#include <filesystem>



namespace emberBackendInterface
{
    // Forward declerations:
    class IBuffer;
    class ICompute;
    class IComputeShader;
    class IGui;
    class IMaterial;
    class IMesh;
    class IShaderProperties;
    class ITexture;



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
        virtual Uint2 GetSurfaceExtent() = 0;
        virtual ITexture* GetRenderTexture() = 0;
        virtual float GetDepthBiasConstantFactor() = 0;
        virtual float GetDepthBiasClamp() = 0;
        virtual float GetDepthBiasSlopeFactor() = 0;

        // Setters:
        virtual void LinkIComputeHandle(emberBackendInterface::ICompute* pICompute) = 0;
        virtual void LinkIGuiHandle(emberBackendInterface::IGui* pIGui) = 0;
        virtual void SetActiveCamera(const Float3& position, const Float4x4& viewMatrix, const Float4x4& projectionMatrix) = 0;
        virtual void SetDepthBiasConstantFactor(float depthBiasConstantFactor) = 0;
        virtual void SetDepthBiasClamp(float depthBiasClamp) = 0;
        virtual void SetDepthBiasSlopeFactor(float depthBiasSlopeFactor) = 0;

        // Functionallity forwarding:
        virtual void CollectGarbage() = 0;
        virtual void WaitDeviceIdle() = 0;

        // Gpu resource factories:
        virtual IBuffer* CreateBuffer(uint32_t count, uint32_t elementSize, const std::string& name, emberCommon::BufferUsage usage) = 0;
        //virtual ITexture* CreateTexture1d(const std::string& name, int width, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data) = 0;
        virtual ITexture* CreateTexture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data) = 0;
        //virtual ITexture* CreateTexture3d(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data) = 0;
        virtual ITexture* CreateTextureCube(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data) = 0;
        virtual IComputeShader* CreateComputeShader(const std::string& name, const std::filesystem::path& computeSpv) = 0;
        virtual IMaterial* CreateMaterial(emberCommon::MaterialType type, const std::string& name, uint32_t renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv) = 0;
        virtual IMesh* CreateMesh(const std::string& name) = 0;
        virtual IShaderProperties* CreateShaderProperties(IComputeShader* pIComputeShader) = 0;
        virtual IShaderProperties* CreateShaderProperties(IMaterial* pIMaterial) = 0;

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