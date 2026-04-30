#pragma once
#include "iRenderer.h"
#include "commonCamera.h"
#include "commonLighting.h"
#include "commonRendererCreateInfo.h"
#include "commonTextureFormat.h"
#include "commonTextureUsage.h"
#include "vulkanDrawCall.h"
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
	struct DrawCall;
	class Mesh;
	class Material;
	class DescriptorSetBinding;
	class StorageBuffer;



	class VULKAN_RENDERER_API Renderer : public emberBackendInterface::IRenderer
	{
	private: // Enums:
		enum class RenderStage
		{
			resourceUpdate = 0,
			preRenderCompute = 1,
			shadow = 2,
			forward = 3,
			postRenderCompute = 4,
			present = 5,
			stageCount = 6
		};
		inline static constexpr std::array<const char*, (int)RenderStage::stageCount> renderStageNames =
		{
			"resourceUpdate",
			"preRenderCompute",
			"shadow",
			"forward",
			"postRenderCompute",
			"present"
		};

	private: // Members:
		// Backend hooks:
		emberBackendInterface::IGui* m_pIGui = nullptr;
		emberBackendInterface::IWindow* m_pIWindow = nullptr;
		Compute* m_pCompute = nullptr;
		
		// Render resources:
		std::vector<CommandPool> m_commandPools;

		// Sync objects:
		std::vector<VkFence> m_frameFences;
		std::vector<VkSemaphore> m_acquireSemaphores;
		std::vector<VkSemaphore> m_resourceUpdateToPreRenderComputeSemaphores;
		std::vector<VkSemaphore> m_preRenderComputeToShadowSemaphores;
		std::vector<VkSemaphore> m_shadowToForwardSemaphores;
		std::vector<VkSemaphore> m_forwardToPostRenderComputeSemaphores;
		std::vector<VkSemaphore> m_postRenderToPresentSemaphores;
		std::vector<VkSemaphore> m_releaseSemaphores;

		// Shadow/Light system:
		float m_depthBiasConstantFactor;
		float m_depthBiasClamp;
		float m_depthBiasSlopeFactor;
		uint32_t m_directionalLightsCount;
		uint32_t m_positionalLightsCount;
		uint32_t m_maxDirectionalLights;
		uint32_t m_maxPositionalLights;
		uint32_t m_shadowMapResolution;
		std::vector<emberCommon::DirectionalLight> m_directionalLights;
		std::vector<emberCommon::PositionalLight> m_positionalLights;

		// Draw calls:
		std::vector<DrawCall> m_staticDrawCalls;	// for draw calls that manage their own call descriptor sets.
		std::vector<DrawCall> m_dynamicDrawCalls;	// for draw calls that get call descriptor sets assigned from a pool.
		std::vector<DrawCall*> m_sortedDrawCallPointers;

		// Render management:
		uint32_t m_frameIndex;
		uint32_t m_imageIndex;  // updated by vkAcquireNextImageKHR(...)
		float m_time;
		float m_deltaTime;
		DescriptorSetBinding* m_pSceneDescriptorSetBinding;
		bool m_rebuildSwapchain;

		// Other:
		emberCommon::Camera m_activeCamera;
		std::vector<std::vector<Mesh*>> m_pendingMeshUpdates;				// one vector per frame in flight. 
		std::vector<std::array<VkDescriptorSet, 3>> m_staticDescriptorSets;	// (global/scen/frame) per frame in flight.

	public: // Methods:
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
		void DrawMesh(emberBackendInterface::IMesh* pMesh, emberBackendInterface::IMaterial* pMaterial, emberBackendInterface::IDescriptorSetBinding* pCallDescriptorSetBinding, const Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true) override;
		emberBackendInterface::IDescriptorSetBinding* DrawMesh(emberBackendInterface::IMesh* pMesh, emberBackendInterface::IMaterial* pMaterial, const Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true) override;

		// Draw instanced:
		void DrawInstanced(uint32_t instanceCount, emberBackendInterface::IBuffer* pInstanceBuffer, emberBackendInterface::IMesh* pMesh, emberBackendInterface::IMaterial* pMaterial, emberBackendInterface::IDescriptorSetBinding* pCallDescriptorSetBinding, const Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true) override;
		emberBackendInterface::IDescriptorSetBinding* DrawInstanced(uint32_t instanceCount, emberBackendInterface::IBuffer* pInstanceBuffer, emberBackendInterface::IMesh* pMesh, emberBackendInterface::IMaterial* pMaterial, const Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true) override;

		// Getters:
		uint32_t GetShadowMapResolution() override;
		Uint2 GetSurfaceExtent() override;
		emberBackendInterface::ITexture* GetRenderTexture() override;
		float GetDepthBiasConstantFactor() override;
		float GetDepthBiasClamp() override;
		float GetDepthBiasSlopeFactor() override;

		// Setters:
		void LinkIComputeHandle(emberBackendInterface::ICompute* pICompute) override;
		void LinkIGuiHandle(emberBackendInterface::IGui* pIGui) override;
		void SetActiveCamera(const Float3& position, const Float4x4& viewMatrix, const Float4x4& projectionMatrix) override;
		void SetDepthBiasConstantFactor(float depthBiasConstantFactor) override;
		void SetDepthBiasClamp(float depthBiasClamp) override;
		void SetDepthBiasSlopeFactor(float depthBiasSlopeFactor) override;

		// Functionality forwarding:
		void CollectGarbage() override;
		void WaitDeviceIdle() override; // needed so core can wait before destroying resource managers and then renderer.

        // Gpu resource factories:
		emberBackendInterface::IBuffer* CreateBuffer(uint32_t count, uint32_t elementSize, emberCommon::BufferUsage usage) override;
		//emberBackendInterface::ITexture* CreateTexture1d(int width, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data) override;
		emberBackendInterface::ITexture* CreateTexture2d(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data) override;
		//emberBackendInterface::ITexture* CreateTexture3d(int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data) override;
		emberBackendInterface::ITexture* CreateTextureCube(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data) override;
		emberBackendInterface::IComputeShader* CreateComputeShader(const std::string& name, const std::filesystem::path& computeSpv) override;
		emberBackendInterface::IMaterial* CreateForwardMaterial(const std::string& name, emberCommon::RenderMode renderMode, uint32_t renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv) override;
		emberBackendInterface::IMesh* CreateMesh() override;
		emberBackendInterface::IDescriptorSetBinding* CreateComputeCallDescriptorSetBinding(emberBackendInterface::IComputeShader* pIComputeShader) override;
		emberBackendInterface::IDescriptorSetBinding* CreateDrawCallDescriptorSetBinding(emberBackendInterface::IMaterial* pIMaterial) override;

		// Vulkan handle passthrough for API coupling:
		void* GetVkInstance() const override;
		void* GetVkPhysicalDevice() const override;
		void* GetVkDevice() const override;
		void* GetPresentVkRenderPass() const override;
		void* GetVkDescriptorPool() const override;
		void* GetGraphicsVkQueue() const override;
		void* GetColorSampler() const override;
		uint32_t GetGraphicsVkQueueFamilyIndex() const override;
		uint32_t GetSwapchainImageCount() const override;
		uint32_t GetFramesInFlight() const override;

		// Backend only:
		void QueueMeshForUpdate(Mesh* pMesh);
		void RemoveQueuedMeshUpdate(Mesh* pMesh);
		void ReplaceQueuedMeshUpdate(Mesh* pOldMesh, Mesh* pNewMesh);
		std::array<VkDescriptorSet, 3>& GetStaticDescriptorSets(uint32_t frameIndex);

	private: // Methods:
		// Reset render state:
		void ResetLights();
		void ResetDrawCalls();

		void RebuildSwapchain();
		bool AcquireImage();
		void SortDrawCallPointers();
		void UpdateShaderData();

		// Wait for previous frame:
		void WaitForFrameFence();
		void ResetCommandPools();

		// Record commands:
		void RecordResourceUpdateCommands();
		void RecordPreRenderComputeCommands();
		void RecordShadowCommands();
		void RecordForwardCommands();
		void RecordForwardCommandsParallel();
		void RecordPostRenderComputeCommands();
		void RecordPresentCommands();
		void RecordImGuiPresentCommands();

		// Submit commands:
		void SubmitResourceUpdateCommands();
		void SubmitPreRenderComputeCommands();
		void SubmitShadowCommands();
		void SubmitForwardCommands();
		void SubmitForwardCommandsParallel();
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