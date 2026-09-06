#pragma once
#include "iRenderer.h"
#include "commonCamera.h"
#include "commonLighting.h"
#include "commonRendererCreateInfo.h"
#include "commonTextureFormat.h"
#include "commonTextureUsage.h"
#include "vulkanComputeStage.h"
#include "vulkanDeferredGeometryStage.h"
#include "vulkanDeferredLightingStage.h"
#include "vulkanForwardStage.h"
#include "vulkanGizmoStage.h"
#include "vulkanOutlineStage.h"
#include "vulkanPresentStage.h"
#include "vulkanRenderStage.h"
#include "vulkanRendererExport.h"
#include "vulkanResourceUpdateStage.h"
#include "vulkanShadowStage.h"
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
	class IMaterialShaderManager;
	class IMesh;
	class IDescriptorSetBinding;
	class ITexture;
	class IWindow;
}



namespace vulkanRendererBackend
{
	// Forward declarations:
	class CommandPool;
	class Compute;
	class ComputeShader;
	class DepthTexture2d;
	class Mesh;
	class Material;
	class DescriptorSetBinding;
	struct FrameRenderData;
	struct FrameResources;
	class SceneColorTexture2dPair;
	class StorageBuffer;
	class StorageTexture2d;



	class VULKAN_RENDERER_API Renderer : public emberBackendInterface::IRenderer
	{
	private: // Members:
		// Backend hooks:
		emberBackendInterface::IGui* m_pIGui = nullptr;
		emberBackendInterface::IWindow* m_pIWindow = nullptr;
		Compute* m_pCompute = nullptr;

		// Render resources:
		std::vector<FrameResources> m_frameResources;

		// Render stages:
		ResourceUpdateStage m_resourceUpdateStage;
		GizmoStage m_gizmoStage;
		ComputeStage<RenderStage::preRenderCompute> m_preRenderComputeStage;
		OutlineStage m_outlineStage;
		ComputeStage<RenderStage::renderCompute> m_renderComputeStage;
		ShadowStage m_shadowStage;
		DeferredGeometryStage m_deferredGeometryStage;
		DeferredLightingStage m_deferredLightingStage;
		ForwardStage<RenderStage::forwardOpaque> m_forwardOpaqueStage;
		ForwardStage<RenderStage::forwardTransparent> m_forwardTransparentStage;
		ComputeStage<RenderStage::postRenderCompute> m_postRenderComputeStage;
		PresentStage m_presentStage;

		// Sync objects:
		std::vector<VkFence> m_frameFences;
		std::vector<VkSemaphore> m_acquireSemaphores;
		std::vector<VkSemaphore> m_resourceUpdateToPreRenderComputeSemaphores;
		std::vector<VkSemaphore> m_resourceUpdateToGizmoSemaphores;
		std::vector<VkSemaphore> m_preRenderComputeToShadowSemaphores;
		std::vector<VkSemaphore> m_preRenderComputeToDeferredGeometrySemaphores;
		std::vector<VkSemaphore> m_preRenderComputeToOutlineSemaphores;
		std::vector<VkSemaphore> m_shadowToDeferredLightingSemaphores;
		std::vector<VkSemaphore> m_deferredGeometryToDeferredLightingSemaphores;
		std::vector<VkSemaphore> m_deferredLightingToForwardOpaqueSemaphores;
		std::vector<VkSemaphore> m_forwardOpaqueToForwardTransparentSemaphores;
		std::vector<VkSemaphore> m_forwardTransparentToPostRenderComputeSemaphores;
		std::vector<VkSemaphore> m_outlineToRenderComputeSemaphores;
		std::vector<VkSemaphore> m_renderComputeToPostRenderComputeSemaphores;
		std::vector<VkSemaphore> m_gizmoToPresentSemaphores;
		std::vector<VkSemaphore> m_postRenderComputeToPresentSemaphores;
		std::vector<VkSemaphore> m_releaseSemaphores;

		// Render Graph:
		// ResourceUpdate
		// ├─> PreRenderCompute ─┬─> Shadow ───────────┐
		// │                     ├─> DeferredGeometry ─┴─> DeferredLighting ─> ForwardOpaque ─> ForwardTransparent ─┐
		// │                     └─> Outline ─> RenderCompute ──────────────────────────────────────────────────────┴─> PostRenderCompute ─┐
		// └─> Gizmo ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────> Present

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
		uint32_t m_frameIndex = 0;
		uint32_t m_imageIndex = 0;  // updated by vkAcquireNextImageKHR(...)
		float m_time;
		float m_deltaTime;
		DescriptorSetBinding* m_pSceneDescriptorSetBinding;
		bool m_rebuildSwapchain;

		// Other:
		emberCommon::Camera m_activeCamera;
		std::vector<FrameRenderData> m_frameRenderData;
		std::vector<std::array<VkDescriptorSet, 3>> m_staticDescriptorSets;	// (global/scen/frame) per frame in flight.

		// Scene textures:
		std::unique_ptr<SceneColorTexture2dPair> m_pSceneColorTexturePair;
		std::vector<std::unique_ptr<DepthTexture2d>> m_pSceneDepthTextures;
		std::vector<std::unique_ptr<StorageTexture2d>> m_pExpandedOutlineMaskTextures;
		std::vector<std::unique_ptr<StorageTexture2d>> m_pHorizontalExpandedOutlineMaskTextures;

	public: // Methods:
		// Constructor/Destructor:
		Renderer(const emberCommon::RendererCreateInfo& createInfo, emberBackendInterface::IWindow* pIWindow);
		~Renderer();

		// Non-copyable:
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

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
		// Ember::ToDo: emberBackendInterface::IDescriptorSetBinding* DrawOutline(...) is missing.
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

		// Gpu resource destruction:
		void DestroyComputeShader(emberBackendInterface::IComputeShader* pIComputeShader) override;

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
		void CreateSceneTextures(uint32_t renderWidth, uint32_t renderHeight);
		void RebuildSwapchain();
		bool AcquireImage();
		void SortDrawCallPointers();
		void QueueRendererOwnedComputeShaders();
		void UpdateShaderData();

		// Submit commands:
		void SubmitResourceUpdateCommands();
		void SubmitPreRenderComputeCommands();
		void SubmitOutlineCommands();
		void SubmitRenderComputeCommands();
		void SubmitShadowCommands();
		void SubmitDeferredGeometryCommands();
		void SubmitDeferredLightingCommands();
		void SubmitForwardOpaqueCommands();
		void SubmitForwardTransparentCommands();
		void SubmitGizmoCommands();
		void SubmitPostRenderComputeCommands();
		void SubmitPresentCommands();
		bool PresentImage();

		// Sync objects management:
		void CreateFences();
		void CreateSemaphores();
		void DestroyFences();
		void DestroySemaphores();

		// Internal getters:
		CommandPool& GetCommandPool(int frameIndex, RenderStage renderStage);
		CommandPool& GetCommandPool(int frameIndex, int renderStage);
	};
}