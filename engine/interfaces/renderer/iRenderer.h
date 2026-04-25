#pragma once
#include "emberMath.h"
#include "commonBufferUsage.h"
#include "commonLighting.h"
#include "commonPipelineState.h"
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
    class IDescriptorSetBinding;
    class ITexture;



    class IRenderer
    {
    public: // Methods:
        // Virtual destructor for v-table:
        virtual ~IRenderer() = default;

        // Main render loop:
        virtual void RenderFrame(float time, float deltaTime) = 0;

        // Lightsources:
        virtual void AddDirectionalLight(const Float3& direction, float intensity, const Float3& color, emberCommon::ShadowType shadowType, const Float4x4& worldToClipMatrix) = 0;
        virtual void AddPositionalLight(const Float3& position, float intensity, const Float3& color, emberCommon::ShadowType shadowType, float blendStart, float blendEnd, const Float4x4& worldToClipMatrix) = 0;

        // Draw mesh:
        virtual void DrawMesh(IMesh* pMesh, IMaterial* pMaterial, IDescriptorSetBinding* pCallDescriptorSetBinding, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows) = 0;
        virtual IDescriptorSetBinding* DrawMesh(IMesh* pMesh, IMaterial* pMaterial, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows) = 0;

        // Draw instanced:
        virtual void DrawInstanced(uint32_t instanceCount, IBuffer* pInstanceBuffer, IMesh* pMesh, IMaterial* pMaterial, IDescriptorSetBinding* pCallDescriptorSetBinding, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows) = 0;
        virtual IDescriptorSetBinding* DrawInstanced(uint32_t instanceCount, IBuffer* pInstanceBuffer, IMesh* pMesh, IMaterial* pMaterial, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows) = 0;

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
        virtual IBuffer* CreateBuffer(uint32_t count, uint32_t elementSize, emberCommon::BufferUsage usage) = 0;
        //virtual ITexture* CreateTexture1d(int width, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data) = 0;
        virtual ITexture* CreateTexture2d(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data) = 0;
        //virtual ITexture* CreateTexture3d(int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data) = 0;
        virtual ITexture* CreateTextureCube(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data) = 0;
        virtual IComputeShader* CreateComputeShader(const std::string& name, const std::filesystem::path& computeSpv) = 0;
        virtual IMaterial* CreateForwardMaterial(const std::string& name, emberCommon::RenderMode renderMode, uint32_t renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv) = 0;
        virtual IMesh* CreateMesh() = 0;
        virtual IDescriptorSetBinding* CreateComputeCallDescriptorSetBinding(IComputeShader* pIComputeShader) = 0;
        virtual IDescriptorSetBinding* CreateDrawCallDescriptorSetBinding(IMaterial* pIMaterial) = 0;

        // Vulkan handle passthrough for API coupling:
        virtual void* GetVkInstance() const = 0;
        virtual void* GetVkPhysicalDevice() const = 0;
        virtual void* GetVkDevice() const = 0;
        virtual void* GetPresentVkRenderPass() const = 0;
        virtual void* GetVkDescriptorPool() const = 0;
        virtual void* GetGraphicsVkQueue() const = 0;
        virtual void* GetColorSampler() const = 0;
        virtual uint32_t GetGraphicsVkQueueFamilyIndex() const = 0;
        virtual uint32_t GetSwapchainImageCount() const = 0;
        virtual uint32_t GetFramesInFlight() const = 0;
    };
}