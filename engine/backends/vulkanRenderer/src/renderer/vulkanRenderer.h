#pragma once
#include "iRenderer.h"
#include "commonCamera.h"
#include "commonLighting.h"
#include "commonRendererCreateInfo.h"
#include "commonTextureFormat.h"
#include "commonTextureUsage.h"
#include "vulkanFrameExecutionData.h"
#include "vulkanRendererExport.h"
#include <array>
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>



// Forward declarations:
namespace emberBackendInterface
{
	class IBuffer;
	class IGui;
	class ICompute;
	class IMaterial;
	class IMaterialManager;
	class IMesh;
	class IDescriptorSetBinding;
	class ITexture;
	class IWindow;
}



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Compute;
	class ComputeShader;
	class DescriptorSetBinding;
	class Mesh;
	class Material;
	class RenderGraph;
	class RenderTargetResources;
	struct FrameRenderData;
	struct FrameResources;
	class StorageBuffer;



	class VULKAN_RENDERER_API Renderer : public emberBackendInterface::IRenderer
	{
	private: // Members:
		// Backend hooks:
		emberBackendInterface::IGui* m_pIGui = nullptr;
		emberBackendInterface::IWindow* m_pIWindow = nullptr;
		Compute* m_pCompute = nullptr;

		// Render resources:
		std::unique_ptr<RenderGraph> m_pRenderGraph;
		std::vector<FrameResources> m_frameResources;
		std::unique_ptr<RenderTargetResources> m_pRenderTargets;

		// Shadow/Light system:
		float m_depthBiasConstantFactor;
		float m_depthBiasClamp;
		float m_depthBiasSlopeFactor;
		Float4 m_outlineColor;
		int m_outlineThickness;
		uint32_t m_directionalLightsCount;
		uint32_t m_positionalLightsCount;
		uint32_t m_previousDirectionalLightsCount;
		uint32_t m_previousPositionalLightsCount;
		uint32_t m_maxDirectionalLights;
		uint32_t m_maxPositionalLights;
		uint32_t m_shadowMapResolution;
		std::vector<emberCommon::DirectionalLight> m_directionalLights;
		std::vector<emberCommon::PositionalLight> m_positionalLights;
		std::vector<emberCommon::DirectionalLight> m_previousDirectionalLights;
		std::vector<emberCommon::PositionalLight> m_previousPositionalLights;

		// Render management:
		FrameExecutionData m_frameExecutionData;
		DescriptorSetBinding* m_pSceneDescriptorSetBinding;
		bool m_rebuildSwapchain;

		// Other:
		emberCommon::Camera m_activeCamera;
		std::vector<FrameRenderData> m_frameRenderData;
		std::vector<std::array<VkDescriptorSet, 3>> m_staticDescriptorSets;	// (global/scen/frame) per frame in flight.

	public: // Methods:
		// Constructor/Destructor:
		Renderer(const emberCommon::RendererCreateInfo& createInfo, emberBackendInterface::IWindow* pIWindow);
		~Renderer();

		// Non-copyable:
		Renderer(const Renderer& other) = delete;
		Renderer& operator=(const Renderer& other) = delete;

		// Movable:
		Renderer(Renderer&& other) noexcept;
		Renderer& operator=(Renderer&& other) noexcept;

		// Main render loop:
		void RenderFrame(float time, float deltaTime) override;

		// Lightsources:
		void AddDirectionalLight(const Float3& direction, float intensity, const Float3& color, emberCommon::ShadowType shadowType, const Float4x4& worldToClipMatrix) override;
		void AddPositionalLight(const Float3& position, float intensity, const Float3& color, emberCommon::ShadowType shadowType, float blendStart, float blendEnd, const Float4x4& worldToClipMatrix) override;

		// Draw mesh:
        void DrawOutline(const Float4x4& localToWorldMatrix, emberBackendInterface::IMesh* pIMesh, uint32_t instanceCount) override;
		void DrawMesh(const Float4x4& localToWorldMatrix, emberBackendInterface::IMesh* pIMesh, emberBackendInterface::IMaterial* pIMaterial, emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding, emberCommon::CullMode cullMode, bool receiveShadows, uint32_t instanceCount) override;
		emberBackendInterface::IDescriptorSetBinding* DrawMesh(const Float4x4& localToWorldMatrix,emberBackendInterface::IMesh* pIMesh, emberBackendInterface::IMaterial* pIMaterial, emberCommon::CullMode cullMode, bool receiveShadows, uint32_t instanceCount) override;
		void DrawMeshShadow(const Float4x4& localToWorldMatrix,emberBackendInterface::IMesh* pIMesh, emberBackendInterface::IMaterial* pIMaterial, emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding, uint32_t instanceCount) override;
		emberBackendInterface::IDescriptorSetBinding* DrawMeshShadow(const Float4x4& localToWorldMatrix,emberBackendInterface::IMesh* pIMesh, emberBackendInterface::IMaterial* pIMaterial, uint32_t instanceCount) override;
		void DrawGizmo(const Float4x4& localToWorldMatrix,emberBackendInterface::IMesh* pIMesh, emberBackendInterface::IMaterial* pIMaterial, emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding, emberCommon::CullMode cullMode, uint32_t instanceCount) override;
		emberBackendInterface::IDescriptorSetBinding* DrawGizmo(const Float4x4& localToWorldMatrix,emberBackendInterface::IMesh* pIMesh, emberBackendInterface::IMaterial* pIMaterial, emberCommon::CullMode cullMode, uint32_t instanceCount) override;

		// Getters:
		bool TryGetDirectionalLight(emberCommon::DirectionalLight& directionalLight, uint32_t index) const override;
		bool TryGetPositionalLight(emberCommon::PositionalLight& positionalLight, uint32_t index) const override;
		uint32_t GetShadowMapResolution() override;
		Uint2 GetSurfaceExtent() override;
		emberBackendInterface::ITexture* GetFinalRenderTexture() override;
		emberBackendInterface::ITexture* GetGizmoTexture() override;
		float GetDepthBiasConstantFactor() override;
		float GetDepthBiasClamp() override;
		float GetDepthBiasSlopeFactor() override;
		const Float4& GetOutlineColor() const override;
		int GetOutlineThickness() const override;
		uint32_t GetFrameIndex() const override;
		bool IsFrameFinished(uint32_t frameIndex) const override;

		// Setters:
		void LinkIComputeHandle(emberBackendInterface::ICompute* pICompute) override;
		void LinkIGuiHandle(emberBackendInterface::IGui* pIGui) override;
		void SetActiveCamera(const Float3& position, const Float4x4& viewMatrix, const Float4x4& projectionMatrix) override;
		void SetDepthBiasConstantFactor(float depthBiasConstantFactor) override;
		void SetDepthBiasClamp(float depthBiasClamp) override;
		void SetDepthBiasSlopeFactor(float depthBiasSlopeFactor) override;
		void SetOutlineColor(const Float4& outlineColor) override;
		void SetOutlineThickness(int outlineThickness) override;

		// Functionality forwarding:
		void CollectGarbage() override;
		void WaitDeviceIdle() override; // needed so core can wait before destroying resource managers and then renderer.
		void WaitForFrameFinished(uint32_t frameIndex) override;

		// Vulkan handle passthrough for API coupling:
		void* GetVkInstance() const override;
		void* GetVkPhysicalDevice() const override;
		void* GetVkDevice() const override;
		void* GetPresentVkRenderPass() const override;
		void* GetGraphicsVkQueue() const override;
		void* GetColorSampler() const override;
		uint32_t GetGraphicsVkQueueFamilyIndex() const override;
		uint32_t GetSwapchainImageCount() const override;
		uint32_t GetFramesInFlight() const override;

        // Debugging:
		void DumpVmaBufferAllocations() const;
		void DumpVmaImageAllocations() const;

		// Backend only:
		void QueueMeshForUpdate(Mesh* pMesh);
		void RemoveQueuedMeshUpdate(Mesh* pMesh);
		void ReplaceQueuedMeshUpdate(Mesh* pOldMesh, Mesh* pNewMesh);
		std::array<VkDescriptorSet, 3>& GetStaticDescriptorSets(uint32_t frameIndex);

	private: // Methods:
		// Resets:
		void ResetFrameCalls();

		// Other:
		void RebuildSwapchain();
		bool AcquireImage();
		void SortDrawCallPointers();
		void QueueRendererOwnedComputeShaders();
		void UpdateShaderData();
		bool PresentImage();
	};
}