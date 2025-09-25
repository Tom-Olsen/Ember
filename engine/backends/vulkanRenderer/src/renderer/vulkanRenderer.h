#pragma once
#include "iRenderer.h"
#include "commonCamera.h"
#include "commonLighting.h"
#include "vulkanDrawCall.h"
#include "vulkanRendererExport.h"
#include <array>
#include <memory>
#include <vector>



// Forward declarations:
typedef struct VkFence_T* VkFence;
typedef struct VkSemaphore_T* VkSemaphore;
typedef struct VkPipeline_T* VkPipeline;
typedef struct VkPipelineLayout_T* VkPipelineLayout;
namespace emberEngine
{
	struct RendererCreateInfo;
}
namespace emberBackendInterface
{
	class IDearImGui;
}



namespace vulkanRendererBackend
{
	// Forward declarations:
	class CommandPool;
	class ComputeShader;
	struct DrawCall;
	class Mesh;
	class Material;
	class ShaderProperties;
	class StorageBuffer;



	class VULKAN_RENDERER_API Renderer : public emberBackendInterface::IRenderer
	{
	private: // Enums:
		enum class RenderStage
		{
			preRenderCompute = 0,
			shadow = 1,
			forward = 2,
			postRenderCompute = 3,
			present = 4,
			stageCount = 5
		};
		inline static constexpr std::array<const char*, (int)RenderStage::stageCount> renderStageNames =
		{
			"preRenderCompute",
			"shadow",
			"forward",
			"postRenderCompute",
			"present"
		};

	private: // Members:
		// Backend hooks:
		emberBackendInterface::IDearImGui* m_pIDearImGui;
		
		// Render resources:
		std::vector<CommandPool> m_commandPools;

		// Sync objects:
		std::vector<VkFence> m_frameFences;
		std::vector<VkSemaphore> m_acquireSemaphores;
		std::vector<VkSemaphore> m_preRenderComputeToShadowSemaphores;
		std::vector<VkSemaphore> m_shadowToForwardSemaphores;
		std::vector<VkSemaphore> m_forwardToPostRenderComputeSemaphores;
		std::vector<VkSemaphore> m_postRenderToPresentSemaphores;
		std::vector<VkSemaphore> m_releaseSemaphores;

		// Shadow/Light system:
		std::unique_ptr<Material> m_pShadowMaterial;
		VkPipelineLayout m_shadowPipelineLayout;
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
		std::array<Float4x4, 6> m_pointLightRotationMatrices;

		// Present render pass caching:
		std::unique_ptr<Mesh> m_pPresentMesh;
		std::unique_ptr<Material> m_pPresentMaterial;
		std::unique_ptr<ShaderProperties> m_pPresentShaderProperties;
		VkPipeline m_presentPipeline = nullptr;
		VkPipelineLayout m_presentPipelineLayout = nullptr;
		uint32_t m_presentBindingCount = 0;

		// DrawCall management:
		emberCommon::Camera m_activeCamera;
		std::vector<DrawCall> m_staticDrawCalls;
		std::vector<DrawCall> m_dynamicDrawCalls;
		std::vector<DrawCall*> m_sortedDrawCallPointers;

		// Maybe move somewhere else?
		std::unique_ptr<Material> m_pDefaultMaterial;
		std::unique_ptr<Material> m_pErrorMaterial;
		std::unique_ptr<ComputeShader> m_pGammaCorrectionComputeShader;

		// Render management:
		uint32_t m_imageIndex;  // updated by vkAcquireNextImageKHR(...)
		float m_time;
		float m_deltaTime;
		bool m_rebuildSwapchain;

	public: // Methods:
		Renderer(const emberEngine::RendererCreateInfo& createInfo);
		~Renderer();

		// Non-copyable:
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		// Non-movable:
		Renderer(Renderer&& other) noexcept = delete;
		Renderer& operator=(Renderer&& other) noexcept = delete;

		// Main render call:
		void RenderFrame(int windowWidth, int windowHeight, float time, float deltaTime) override;

		// Lightsources:
		void AddDirectionalLight(const Float3& direction, float intensity, const Float3& color, emberCommon::ShadowType shadowType, const Float4x4& worldToClipMatrix) override;
		void AddPositionalLight(const Float3& position, float intensity, const Float3& color, emberCommon::ShadowType shadowType, float blendStart, float blendEnd, const Float4x4& worldToClipMatrix) override;

		// Draw mesh:
		void DrawMesh(Mesh* pMesh, Material* pMaterial, ShaderProperties* pShaderProperties, const Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true) override;
		ShaderProperties* DrawMesh(Mesh* pMesh, Material* pMaterial, const Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true) override;

		// Draw instanced:
		void DrawInstanced(uint32_t instanceCount, StorageBuffer* pInstanceBuffer, Mesh* pMesh, Material* pMaterial, ShaderProperties* pShaderProperties, const Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true) override;
		ShaderProperties* DrawInstanced(uint32_t instanceCount, StorageBuffer* pInstanceBuffer, Mesh* pMesh, Material* pMaterial, const Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true) override;

		// Getters:
		float GetDeptBiasConstantFactor();
		float GetDeptBiasClamp();
		float GetDeptBiasSlopeFactor();

		// Setters:
		void SetActiveCamera(const Float3& position, const Float4x4& viewMatrix, const Float4x4& projectionMatrix);
		void SetDepthBiasConstantFactor(float depthBiasConstantFactor);
		void SetDepthBiasClamp(float depthBiasClamp);
		void SetDepthBiasSlopeFactor(float depthBiasSlopeFactor);

	private: // Methods:
		// Reset render state:
		void ResetLights();
		void ResetDrawCalls();

		void RebuildSwapchain();
		bool AcquireImage();
		void SortDrawCallPointers();

		// Wait for previous frame:
		void WaitForFrameFence();
		void ResetCommandPools();

		// Record commands:
		void RecordPreRenderComputeCommands();
		void RecordShadowCommands();
		void RecordForwardCommands();
		void RecordForwardCommandsParallel();
		void RecordPostRenderComputeCommands();
		void RecordPresentCommands();
		void RecordImGuiPresentCommands();

		// Submit commands:
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
		
		CommandPool& GetCommandPool(int frameIndex, RenderStage renderStage);
		CommandPool& GetCommandPool(int frameIndex, int renderStage);

		Mesh* CreateFullScreenRenderQuad();
	};
}