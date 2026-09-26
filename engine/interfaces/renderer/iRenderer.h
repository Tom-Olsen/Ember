#pragma once
#include "commonCullMode.h"
#include "commonLighting.h"
#include "emberMath.h"



namespace emberBackendInterface
{
    // Forward declarations:
	class IBuffer;
	class ICompute;
	class IComputeShader;
	class IDescriptorSetBinding;
	class IGui;
	class IMaterial;
	class IMesh;
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
        virtual IDescriptorSetBinding* DrawOutline(IMesh* pIMesh, uint32_t instanceCount) = 0;
        virtual void DrawMesh(const Float3& worldPosition, IMesh* pIMesh, IMaterial* pIMaterial, IDescriptorSetBinding* pCallDescriptorSetBinding, emberCommon::CullMode cullMode, bool receiveShadows, uint32_t instanceCount) = 0;
        virtual IDescriptorSetBinding* DrawMesh(const Float3& worldPosition, IMesh* pIMesh, IMaterial* pIMaterial, emberCommon::CullMode cullMode, bool receiveShadows, uint32_t instanceCount) = 0;
        virtual void DrawMeshShadow(IMesh* pIMesh, IMaterial* pIMaterial, IDescriptorSetBinding* pICallDescriptorSetBinding, uint32_t instanceCount) = 0;
        virtual IDescriptorSetBinding* DrawMeshShadow(IMesh* pIMesh, IMaterial* pMaterial, uint32_t instanceCount) = 0;
        virtual void DrawGizmo(const Float3& worldPosition, IMesh* pIMesh, IMaterial* pIMaterial, IDescriptorSetBinding* pCallDescriptorSetBinding, emberCommon::CullMode cullMode, uint32_t instanceCount) = 0;
        virtual IDescriptorSetBinding* DrawGizmo(const Float3& worldPosition, IMesh* pIMesh, IMaterial* pIMaterial, emberCommon::CullMode cullMode, uint32_t instanceCount) = 0;

        // Getters:
        virtual bool TryGetDirectionalLight(emberCommon::DirectionalLight& directionalLight, uint32_t index) const = 0;
        virtual bool TryGetPositionalLight(emberCommon::PositionalLight& positionalLight, uint32_t index) const = 0;
        virtual uint32_t GetShadowMapResolution() = 0;
        virtual Uint2 GetSurfaceExtent() = 0;
        virtual ITexture* GetFinalRenderTexture() = 0;
        virtual ITexture* GetGizmoTexture() = 0;
        virtual float GetDepthBiasConstantFactor() = 0;
        virtual float GetDepthBiasClamp() = 0;
        virtual float GetDepthBiasSlopeFactor() = 0;
        virtual const Float4& GetOutlineColor() const = 0;
        virtual int GetOutlineThickness() const = 0;
        virtual uint32_t GetFrameIndex() const = 0;
        virtual bool IsFrameFinished(uint32_t frameIndex) const = 0;

        // Setters:
        virtual void LinkIComputeHandle(emberBackendInterface::ICompute* pICompute) = 0;
        virtual void LinkIGuiHandle(emberBackendInterface::IGui* pIGui) = 0;
        virtual void SetActiveCamera(const Float3& position, const Float4x4& viewMatrix, const Float4x4& projectionMatrix) = 0;
        virtual void SetDepthBiasConstantFactor(float depthBiasConstantFactor) = 0;
        virtual void SetDepthBiasClamp(float depthBiasClamp) = 0;
        virtual void SetDepthBiasSlopeFactor(float depthBiasSlopeFactor) = 0;
        virtual void SetOutlineColor(const Float4& outlineColor) = 0;
        virtual void SetOutlineThickness(int outlineThickness) = 0;

        // Functionallity forwarding:
        virtual void CollectGarbage() = 0;
        virtual void WaitDeviceIdle() = 0;
        virtual void WaitForFrameFinished(uint32_t frameIndex) = 0;

        // Vulkan handle passthrough for API coupling:
        virtual void* GetVkInstance() const = 0;
        virtual void* GetVkPhysicalDevice() const = 0;
        virtual void* GetVkDevice() const = 0;
        virtual void* GetPresentVkRenderPass() const = 0;
        virtual void* GetGraphicsVkQueue() const = 0;
        virtual void* GetColorSampler() const = 0;
        virtual uint32_t GetGraphicsVkQueueFamilyIndex() const = 0;
        virtual uint32_t GetSwapchainImageCount() const = 0;
        virtual uint32_t GetFramesInFlight() const = 0;

        // Debugging:
		virtual void DumpVmaBufferAllocations() const = 0;
		virtual void DumpVmaImageAllocations() const = 0;
    };
}