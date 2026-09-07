#include "vulkanRenderer.h"
#include "descriptorSetMacros.h"
#include "emberMath.h"
#include "iGui.h"
#include "iWindow.h"
#include "logger.h"
#include "parallelThreadPool.h"
#include "profiler.h"
#include "shadowConstants.h"
#include "vmaImage.h"
#include "vulkanAccessMask.h"
#include "vulkanAllocationTracker.h"
#include "vulkanAsyncCompute.h"
#include "vulkanCommandPool.h"
#include "vulkanCompute.h"
#include "vulkanComputeShader.h"
#include "vulkanContext.h"
#include "vulkanConvertTextureFormat.h"
#include "vulkanDefaultGpuResources.h"
#include "vulkanDeferredDrawCall.h"
#include "vulkanDeferredGeometryRenderPass.h"
#include "vulkanDeferredLightingRenderPass.h"
#include "vulkanDeferredRenderingContract.h"
#include "vulkanDepthTexture2d.h"
#include "vulkanDepthTexture2dArray.h"
#include "vulkanDescriptorPoolManager.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanForwardDrawCall.h"
#include "vulkanForwardOpaqueRenderPass.h"
#include "vulkanForwardTransparentRenderPass.h"
#include "vulkanFrameContext.h"
#include "vulkanFrameDescriptorSetLayout.h"
#include "vulkanFrameRenderData.h"
#include "vulkanFrameResources.h"
#include "vulkanGarbageCollector.h"
#include "vulkanGBufferTexture2d.h"
#include "vulkanGizmoDrawCall.h"
#include "vulkanGizmoRenderPass.h"
#include "vulkanGlobalDescriptorSetLayout.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include "vulkanMaterial.h"
#include "vulkanMaterialManager.h"
#include "vulkanMaterialShaderManager.h"
#include "vulkanMesh.h"
#include "vulkanMidRenderCompute.h"
#include "vulkanOutlineDrawCall.h"
#include "vulkanOutlineRenderPass.h"
#include "vulkanPoolManager.h"
#include "vulkanPostRenderCompute.h"
#include "vulkanPreRenderCompute.h"
#include "vulkanPresentRenderPass.h"
#include "vulkanRenderGraph.h"
#include "vulkanRenderPassManager.h"
#include "vulkanRenderStage.h"
#include "vulkanRenderTexture2d.h"
#include "vulkanSampleTexture2d.h"
#include "vulkanSampleTexture3d.h"
#include "vulkanSampleTextureCube.h"
#include "vulkanSampler.h"
#include "vulkanSceneColorTexture2dPair.h"
#include "vulkanSceneDescriptorSetLayout.h"
#include "vulkanScreenSpaceCompute.h"
#include "vulkanShadowDrawCall.h"
#include "vulkanShadowRenderPass.h"
#include "vulkanSingleTimeCommand.h"
#include "vulkanStorageBuffer.h"
#include "vulkanStorageSampleTexture2d.h"
#include "vulkanStorageSampleTexture3d.h"
#include "vulkanStorageTexture2d.h"
#include "vulkanStorageTexture3d.h"
#include "vulkanSurface.h"
#include "vulkanSwapchain.h"
#include "vulkanVertexBuffer.h"
#include <algorithm>
#include <array>
#include <assert.h>
#include <cstdlib>
#include <stdexcept>
#include <string>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	Renderer::Renderer(const emberCommon::RendererCreateInfo& createInfo, emberBackendInterface::IWindow* pIWindow)
	{
		m_pIWindow = pIWindow;
		m_maxDirectionalLights = math::Clamp(createInfo.maxDirectionalLights, uint32_t(1), uint32_t(MAX_DIR_LIGHTS));
		m_maxPositionalLights = math::Clamp(createInfo.maxPositionalLights, uint32_t(1), uint32_t(MAX_POS_LIGHTS));
		m_shadowMapResolution = math::Clamp(createInfo.shadowMapResolution, uint32_t(1), uint32_t(SHADOW_MAP_RESOLUTION));

		// Initialization:
		Context::Init(createInfo, pIWindow, this);
		SingleTimeCommand::Init();
		GarbageCollector::Init();
		DescriptorPoolManager::Init();
		DefaultGpuResources::InitSamplers();
		PoolManager::Init();
		CreateSceneTextures(createInfo.renderWidth, createInfo.renderHeight);
		RenderPassManager::Init(createInfo.renderWidth, createInfo.renderHeight, m_shadowMapResolution, m_maxDirectionalLights + m_maxPositionalLights, m_pSceneColorTexturePair->GetRenderTargetTextures(), m_pSceneDepthTextures);
		GlobalDescriptorSetLayout::Init();
		SceneDescriptorSetLayout::Init();
		FrameDescriptorSetLayout::Init();
		DefaultGpuResources::Init();

		m_time = 0.0f;
		m_deltaTime = 0.0f;
		m_rebuildSwapchain = false;

		// Render resources:
		m_pRenderGraph = std::make_unique<RenderGraph>();
		m_frameRenderData.resize(Context::GetFramesInFlight());
		m_frameResources.reserve(Context::GetFramesInFlight());
		for (int frameIndex = 0; frameIndex < Context::GetFramesInFlight(); frameIndex++)
			m_frameResources.emplace_back(emberTaskSystem::ParallelThreadPool::GetCoreCount());

		// Shadow/Light system:
		m_depthBiasConstantFactor = 0.0f;
		m_depthBiasClamp = 0.0f;
		m_depthBiasSlopeFactor = 3.0f;
		m_outlineColor = Float4::orange;
		m_outlineThickness = 3;
		m_directionalLightsCount = 0;
		m_positionalLightsCount = 0;
		m_previousDirectionalLightsCount = 0;
		m_previousPositionalLightsCount = 0;
		m_directionalLights.resize(m_maxDirectionalLights);
		m_positionalLights.resize(m_maxPositionalLights);
		m_previousDirectionalLights.resize(m_maxDirectionalLights);
		m_previousPositionalLights.resize(m_maxPositionalLights);

		// Static descriptor sets:
		m_staticDescriptorSets.reserve(Context::GetFramesInFlight());
		for (int frameIndex = 0; frameIndex < Context::GetFramesInFlight(); frameIndex++)
		{
			std::array<VkDescriptorSet, 3> staticDescriptorSets =
			{
				GlobalDescriptorSetLayout::GetVkDescriptorSet(frameIndex),
				SceneDescriptorSetLayout::GetVkDescriptorSet(frameIndex),
				FrameDescriptorSetLayout::GetVkDescriptorSet(frameIndex)
			};
			m_staticDescriptorSets.push_back(staticDescriptorSets);
			m_frameResources[frameIndex].staticDescriptorSets = staticDescriptorSets;
		}

		// Debug naming:
		for (int renderStage = 0; renderStage < (int)RenderStage::stageCount; renderStage++)
			for (int frameIndex = 0; frameIndex < Context::GetFramesInFlight(); frameIndex++)
			{
				std::string name = renderStageNames[renderStage];
				name += "_Frame" + std::to_string(frameIndex);
				NAME_VK_OBJECT(m_frameResources[frameIndex].GetCommandPool(renderStage).GetPrimaryVkCommandPool(), "CommandPool_Primary_" + name);
				NAME_VK_OBJECT(m_frameResources[frameIndex].GetCommandPool(renderStage).GetPrimaryVkCommandBuffer(), "CommandBuffer_Primary_" + name);
				for (int threadIndex = 0; threadIndex < emberTaskSystem::ParallelThreadPool::GetCoreCount(); threadIndex++)
				{
					NAME_VK_OBJECT(m_frameResources[frameIndex].GetCommandPool(renderStage).GetSecondaryVkCommandPool(threadIndex), "CommandPool_Secondary_Thread" + std::to_string(threadIndex) + "_" + name);
					NAME_VK_OBJECT(m_frameResources[frameIndex].GetCommandPool(renderStage).GetSecondaryVkCommandBuffer(threadIndex), "CommandBuffer_Secondary_Thread" + std::to_string(threadIndex) + "_" + name);
				}
			}
	}
	Renderer::~Renderer()
	{
		Context::WaitDeviceIdle();
		m_frameResources.clear();
		m_pRenderGraph.reset();
		FrameDescriptorSetLayout::Clear();
		SceneDescriptorSetLayout::Clear();
		GlobalDescriptorSetLayout::Clear();
		PoolManager::Clear();
		DefaultGpuResources::Clear();
		RenderPassManager::Clear();
		m_pHorizontalExpandedOutlineMaskTextures.clear();
		m_pExpandedOutlineMaskTextures.clear();
		m_pSceneDepthTextures.clear();
		m_pSceneColorTexturePair.reset();
		GarbageCollector::Flush();		// descriptor sets must be destroyed while their parent pools are alive.
		DescriptorPoolManager::Clear();
		GarbageCollector::Clear();
		SingleTimeCommand::Clear();
		Context::Clear();
	}



	// Movable:
	Renderer::Renderer(Renderer&& other) noexcept = default;
	Renderer& Renderer::operator=(Renderer&& other) noexcept = default;



	// Main render call:
	void Renderer::RenderFrame(float time, float deltaTime)
	{
		if (!m_pCompute)
			throw std::runtime_error("vulkanRendererBackend::Renderer::RenderFrame(...) failed. Compute backend is not linked.");

		m_frameIndex = Context::GetFrameIndex();
		m_time = time;
		m_deltaTime = deltaTime;

		// Defer swapchain rebuild until the resize event stream settles.
		if (m_pIWindow->GetIsResizing())
		{
			m_pIWindow->ResetIsResizing();
			m_rebuildSwapchain = true;
			ResetFrameCalls();
			return;
		}

		// Resize swapchain:
		if (m_rebuildSwapchain)
		{
			m_rebuildSwapchain = false;
			Context::ResetFrameIndex();
			RebuildSwapchain();
		}

		// Wait for previous frame fence:
		{
			PROFILE_SCOPE("Renderer::WaitForFrameFence");
			m_pRenderGraph->WaitForFrame(m_frameIndex);
		}

		// Return frame compute shaders callDescriptorSetBindings and decrement their usage count of previous submission with this frame index:
		m_pCompute->RetireFrame(m_frameIndex);

		// Cancel current frame on failed acquisition:
		if (!AcquireImage())
		{
			ResetFrameCalls();
			return;
		}

		// Begin next frame:
		m_pRenderGraph->ResetFrameFence(m_frameIndex);
		m_frameResources[m_frameIndex].ResetCommandPools();
		Context::MarkDeviceBusy();
		m_pSceneColorTexturePair->BeginFrame(m_frameIndex);

		SortDrawCallPointers();
		QueueRendererOwnedComputeShaders();
		UpdateShaderData();

		// Record and submit current frame commands:
		uint32_t shadowMapCount = m_directionalLightsCount + m_positionalLightsCount;
		FrameContext frameContext(m_frameIndex, m_imageIndex, m_time, m_deltaTime, m_shadowMapResolution, shadowMapCount, m_depthBiasConstantFactor, m_depthBiasClamp, m_depthBiasSlopeFactor, m_frameResources[m_frameIndex], m_frameRenderData[m_frameIndex], *m_pSceneColorTexturePair, m_pIGui);
		// ToDo: add computeCalls to frameContext?
		m_pRenderGraph->RecordAndSubmit(frameContext, m_pCompute->GetPreRenderCompute()->GetComputeCalls(), m_pCompute->GetMidRenderCompute()->GetComputeCalls(), m_pCompute->GetScreenSpaceCompute()->GetComputeCalls(), m_pCompute->GetPostRenderCompute()->GetComputeCalls());
		m_pCompute->CommitFrame(m_frameIndex);

		// Finalize frame:
		m_pSceneColorTexturePair->FinalizeFrame(m_frameIndex);
		ResetFrameCalls();

		// Cancel current frame on failed presentation (e.g. window resize):
		if (!PresentImage())
			return;

		Context::UpdateFrameIndex();
	}


	// Add lightsources:
	void Renderer::AddDirectionalLight(const Float3& direction, float intensity, const Float3& color, emberCommon::ShadowType shadowType, const Float4x4& worldToClipMatrix)
	{
		if (m_directionalLightsCount == m_maxDirectionalLights)
		{
			LOG_WARN("Renderer::AddDirectionalLight(...) max directional light capacity reached. Ignoring further directional light sources.");
			return;
		}

		m_directionalLights[m_directionalLightsCount].direction = direction;
		m_directionalLights[m_directionalLightsCount].intensity = intensity;
		m_directionalLights[m_directionalLightsCount].color = color;
		m_directionalLights[m_directionalLightsCount].shadowType = shadowType;
		m_directionalLights[m_directionalLightsCount].worldToClipMatrix = worldToClipMatrix;

		m_directionalLightsCount++;
	}
	void Renderer::AddPositionalLight(const Float3& position, float intensity, const Float3& color, emberCommon::ShadowType shadowType, float blendStart, float blendEnd, const Float4x4& worldToClipMatrix)
	{
		if (m_positionalLightsCount == m_maxPositionalLights)
		{
			LOG_WARN("Renderer::AddPositionalLight(...) max positional light capacity reached. Ignoring further positional light sources.");
			return;
		}

		m_positionalLights[m_positionalLightsCount].position = position;
		m_positionalLights[m_positionalLightsCount].intensity = intensity;
		m_positionalLights[m_positionalLightsCount].color = color;
		m_positionalLights[m_positionalLightsCount].shadowType = shadowType;
		m_positionalLights[m_positionalLightsCount].blendStart = blendStart;
		m_positionalLights[m_positionalLightsCount].blendEnd = blendEnd;
		m_positionalLights[m_positionalLightsCount].worldToClipMatrix = worldToClipMatrix;

		m_positionalLightsCount++;
	}



	// Draw mesh:
    void Renderer::DrawOutline(const Float4x4& localToWorldMatrix, emberBackendInterface::IMesh* pIMesh, uint32_t instanceCount)
    {
		if (!pIMesh)
		{
			LOG_ERROR("vulkanRendererBackend::Renderer::DrawOutline(...) failed. pMesh is nullptr.");
			return;
		}

		// Setup outline call:
		Material* pMaterial = DefaultGpuResources::GetDefaultOutlineMaterial();
		DescriptorSetBindingHandle descriptorSetBindingHandle = PoolManager::CheckOutCallDescriptorSetBindingHandle(pMaterial->GetShader());
		m_frameRenderData[Context::GetFrameIndex()].outlineDrawCalls.emplace_back(localToWorldMatrix, static_cast<Mesh*>(pIMesh), descriptorSetBindingHandle, instanceCount);
    }
	void Renderer::DrawMesh(const Float4x4& localToWorldMatrix, emberBackendInterface::IMesh* pIMesh, emberBackendInterface::IMaterial* pIMaterial, emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding, emberCommon::CullMode cullMode, bool receiveShadows, uint32_t instanceCount)
	{
		// Record static draw call.
		if (!pIMesh)
		{
			LOG_ERROR("vulkanRendererBackend::Renderer::DrawMesh(...) failed. pMesh is nullptr.");
			return;
		}
		if (!pIMaterial)
		{
			LOG_ERROR("vulkanRendererBackend::Renderer::DrawMesh(...) failed. pMaterial is nullptr.");
			return;
		}
		if (!pICallDescriptorSetBinding)
		{
			LOG_ERROR("vulkanRendererBackend::Renderer::DrawMesh(...) failed. pCallDescriptorSetBinding is nullptr.");
			return;
		}

		Material* pMaterial = static_cast<Material*>(pIMaterial);
		DescriptorSetBindingHandle descriptorSetBindingHandle(static_cast<DescriptorSetBinding*>(pICallDescriptorSetBinding));
		switch (pMaterial->GetMaterialPass())
		{
			case emberCommon::MaterialPass::deferredGeometry:
				m_frameRenderData[Context::GetFrameIndex()].deferredDrawCalls.emplace_back(localToWorldMatrix, static_cast<Mesh*>(pIMesh), pMaterial, descriptorSetBindingHandle, cullMode, receiveShadows, instanceCount);
				return;
			case emberCommon::MaterialPass::forward:
				m_frameRenderData[Context::GetFrameIndex()].forwardDrawCalls.emplace_back(localToWorldMatrix, static_cast<Mesh*>(pIMesh), pMaterial, descriptorSetBindingHandle, cullMode, receiveShadows, instanceCount);
				return;
			default:
				throw std::runtime_error("vulkanRendererBackend::Renderer::DrawMesh(...) failed. Material is not a surface material.");
		}
	}
	emberBackendInterface::IDescriptorSetBinding* Renderer::DrawMesh(const Float4x4& localToWorldMatrix, emberBackendInterface::IMesh* pIMesh, emberBackendInterface::IMaterial* pIMaterial, emberCommon::CullMode cullMode, bool receiveShadows, uint32_t instanceCount)
	{
		// Record dynamic draw call.
		if (!pIMesh)
		{
			LOG_ERROR("vulkanRendererBackend::Renderer::DrawMesh(...) failed. pMesh is nullptr.");
			return nullptr;
		}
		if (!pIMaterial)
		{
			LOG_ERROR("vulkanRendererBackend::Renderer::DrawMesh(...) failed. pMaterial is nullptr.");
			return nullptr;
		}

		Material* pMaterial = static_cast<Material*>(pIMaterial);
		switch (pMaterial->GetMaterialPass())
		{
			case emberCommon::MaterialPass::deferredGeometry:
			{
				DescriptorSetBindingHandle descriptorSetBindingHandle = PoolManager::CheckOutCallDescriptorSetBindingHandle(pMaterial->GetShader());
				m_frameRenderData[Context::GetFrameIndex()].deferredDrawCalls.emplace_back(localToWorldMatrix, static_cast<Mesh*>(pIMesh), pMaterial, descriptorSetBindingHandle, cullMode, receiveShadows, instanceCount);
				return descriptorSetBindingHandle.Get();
			}
			case emberCommon::MaterialPass::forward:
			{
				DescriptorSetBindingHandle descriptorSetBindingHandle = PoolManager::CheckOutCallDescriptorSetBindingHandle(pMaterial->GetShader());
				m_frameRenderData[Context::GetFrameIndex()].forwardDrawCalls.emplace_back(localToWorldMatrix, static_cast<Mesh*>(pIMesh), pMaterial, descriptorSetBindingHandle, cullMode, receiveShadows, instanceCount);
				return descriptorSetBindingHandle.Get();
			}
			default:
				throw std::runtime_error("vulkanRendererBackend::Renderer::DrawMesh(...) failed. Material is not a surface material.");
		}
	}
	void Renderer::DrawMeshShadow(const Float4x4& localToWorldMatrix, emberBackendInterface::IMesh* pIMesh, emberBackendInterface::IMaterial* pIMaterial, emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding, uint32_t instanceCount)
	{
		if (pIMesh == nullptr)
		{
			LOG_ERROR("vulkanRendererBackend::Renderer::DrawMeshShadow(...) failed. pMesh is nullptr.");
			return;
		}
		if (pIMaterial == nullptr)
		{
			LOG_ERROR("vulkanRendererBackend::Renderer::DrawMeshShadow(...) failed. pMaterial is nullptr.");
			return;
		}
		if (pICallDescriptorSetBinding == nullptr)
		{
			LOG_ERROR("vulkanRendererBackend::Renderer::DrawMeshShadow(...) failed. pCallDescriptorSetBinding is nullptr.");
			return;
		}

		Material* pMaterial = static_cast<Material*>(pIMaterial);
		if (pMaterial->GetMaterialPass() != emberCommon::MaterialPass::shadow)
			throw std::runtime_error("vulkanRendererBackend::Renderer::DrawMeshShadow(...) failed. Material is not a shadow material.");
		DescriptorSetBindingHandle descriptorSetBindingHandle(static_cast<DescriptorSetBinding*>(pICallDescriptorSetBinding));
		m_frameRenderData[Context::GetFrameIndex()].shadowDrawCalls.emplace_back(localToWorldMatrix, static_cast<Mesh*>(pIMesh), pMaterial, descriptorSetBindingHandle, instanceCount);
	}
	emberBackendInterface::IDescriptorSetBinding* Renderer::DrawMeshShadow(const Float4x4& localToWorldMatrix, emberBackendInterface::IMesh* pIMesh, emberBackendInterface::IMaterial* pIMaterial, uint32_t instanceCount)
	{
		if (pIMesh == nullptr)
		{
			LOG_ERROR("vulkanRendererBackend::Renderer::DrawMeshShadow(...) failed. pMesh is nullptr.");
			return nullptr;
		}
		if (pIMaterial == nullptr)
		{
			LOG_ERROR("vulkanRendererBackend::Renderer::DrawMeshShadow(...) failed. pMaterial is nullptr.");
			return nullptr;
		}

		Material* pMaterial = static_cast<Material*>(pIMaterial);
		if (pMaterial->GetMaterialPass() != emberCommon::MaterialPass::shadow)
			throw std::runtime_error("vulkanRendererBackend::Renderer::DrawMeshShadow(...) failed. Material is not a shadow material.");
		DescriptorSetBindingHandle descriptorSetBindingHandle = PoolManager::CheckOutCallDescriptorSetBindingHandle(pMaterial->GetShader());
		m_frameRenderData[Context::GetFrameIndex()].shadowDrawCalls.emplace_back(localToWorldMatrix, static_cast<Mesh*>(pIMesh), pMaterial, descriptorSetBindingHandle, instanceCount);
		return descriptorSetBindingHandle.Get();
	}
	void Renderer::DrawGizmo(const Float4x4& localToWorldMatrix, emberBackendInterface::IMesh* pIMesh, emberBackendInterface::IMaterial* pIMaterial, emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding, emberCommon::CullMode cullMode, uint32_t instanceCount)
	{
		// Record static gizmo draw call.
		if (!pIMesh)
		{
			LOG_ERROR("vulkanRendererBackend::Renderer::DrawGizmo(...) failed. pMesh is nullptr.");
			return;
		}
		if (!pIMaterial)
		{
			LOG_ERROR("vulkanRendererBackend::Renderer::DrawGizmo(...) failed. pMaterial is nullptr.");
			return;
		}
		if (!pICallDescriptorSetBinding)
		{
			LOG_ERROR("vulkanRendererBackend::Renderer::DrawGizmo(...) failed. pCallDescriptorSetBinding is nullptr.");
			return;
		}

		Material* pMaterial = static_cast<Material*>(pIMaterial);
		DescriptorSetBindingHandle descriptorSetBindingHandle(static_cast<DescriptorSetBinding*>(pICallDescriptorSetBinding));
		m_frameRenderData[Context::GetFrameIndex()].gizmoDrawCalls.emplace_back(localToWorldMatrix, static_cast<Mesh*>(pIMesh), pMaterial, descriptorSetBindingHandle, cullMode, instanceCount);
	}
	emberBackendInterface::IDescriptorSetBinding* Renderer::DrawGizmo(const Float4x4& localToWorldMatrix, emberBackendInterface::IMesh* pIMesh, emberBackendInterface::IMaterial* pIMaterial, emberCommon::CullMode cullMode, uint32_t instanceCount)
	{
		// Record dynamic gizmo draw call.
		if (!pIMesh)
		{
			LOG_ERROR("vulkanRendererBackend::Renderer::DrawGizmo(...) failed. pMesh is nullptr.");
			return nullptr;
		}
		if (!pIMaterial)
		{
			LOG_ERROR("vulkanRendererBackend::Renderer::DrawGizmo(...) failed. pMaterial is nullptr.");
			return nullptr;
		}

		Material* pMaterial = static_cast<Material*>(pIMaterial);
		DescriptorSetBindingHandle descriptorSetBindingHandle = PoolManager::CheckOutCallDescriptorSetBindingHandle(pMaterial->GetShader());
		m_frameRenderData[Context::GetFrameIndex()].gizmoDrawCalls.emplace_back(localToWorldMatrix, static_cast<Mesh*>(pIMesh), pMaterial, descriptorSetBindingHandle, cullMode, instanceCount);
		return descriptorSetBindingHandle.Get();
	}



	// Getters:
	bool Renderer::TryGetDirectionalLight(emberCommon::DirectionalLight& directionalLight, uint32_t index) const
	{
		if (m_directionalLightsCount > 0)
		{
			if (index >= m_directionalLightsCount)
				return false;
			directionalLight = m_directionalLights[index];
			return true;
		}
        // Fallback to previously submitted lights:
		if (index >= m_previousDirectionalLightsCount)
			return false;
		directionalLight = m_previousDirectionalLights[index];
		return true;
	}
	bool Renderer::TryGetPositionalLight(emberCommon::PositionalLight& positionalLight, uint32_t index) const
	{
		if (m_positionalLightsCount > 0)
		{
			if (index >= m_positionalLightsCount)
				return false;
			positionalLight = m_positionalLights[index];
			return true;
		}
        // Fallback to previously submitted lights:
		if (index >= m_previousPositionalLightsCount)
			return false;
		positionalLight = m_previousPositionalLights[index];
		return true;
	}
	uint32_t Renderer::GetShadowMapResolution()
	{
		return m_shadowMapResolution;
	}
	Uint2 Renderer::GetSurfaceExtent()
	{
		return Context::GetSurface()->GetCurrentExtent();
	}
	emberBackendInterface::ITexture* Renderer::GetFinalRenderTexture()
	{
		RenderTexture2d* pRenderTexture = m_pSceneColorTexturePair->GetFinalTexture();
		emberBackendInterface::ITexture* pITexture = static_cast<emberBackendInterface::ITexture*>(pRenderTexture);
		return pITexture;
	}
	emberBackendInterface::ITexture* Renderer::GetGizmoTexture()
	{
		RenderTexture2d* pRenderTexture = RenderPassManager::GetGizmoRenderPass()->GetRenderTexture(m_frameIndex);
		emberBackendInterface::ITexture* pITexture = static_cast<emberBackendInterface::ITexture*>(pRenderTexture);
		return pITexture;
	}
	float Renderer::GetDepthBiasConstantFactor()
	{
		return m_depthBiasConstantFactor;
	}
	float Renderer::GetDepthBiasClamp()
	{
		return m_depthBiasClamp;
	}
	float Renderer::GetDepthBiasSlopeFactor()
	{
		return m_depthBiasSlopeFactor;
	}
	const Float4& Renderer::GetOutlineColor() const
	{
		return m_outlineColor;
	}
	int Renderer::GetOutlineThickness() const
	{
		return m_outlineThickness;
	}
	uint32_t Renderer::GetFrameIndex() const
	{
		return Context::GetFrameIndex();
	}
	bool Renderer::IsFrameFinished(uint32_t frameIndex) const
	{
		return m_pRenderGraph->IsFrameFinished(frameIndex);
	}



	// Setters:
	void Renderer::LinkIComputeHandle(emberBackendInterface::ICompute* pICompute)
	{
		m_pCompute = static_cast<Compute*>(pICompute);
	}
	void Renderer::LinkIGuiHandle(emberBackendInterface::IGui* pIGui)
	{
		m_pIGui = pIGui;
	}
	void Renderer::SetActiveCamera(const Float3& position, const Float4x4& viewMatrix, const Float4x4& projectionMatrix)
	{
		m_activeCamera.position = position;
		m_activeCamera.viewMatrix = viewMatrix;
		m_activeCamera.projectionMatrix = projectionMatrix;
	}
	void Renderer::SetDepthBiasConstantFactor(float depthBiasConstantFactor)
	{
		m_depthBiasConstantFactor = depthBiasConstantFactor;
	}
	void Renderer::SetDepthBiasClamp(float depthBiasClamp)
	{
		m_depthBiasClamp = depthBiasClamp;
	}
	void Renderer::SetDepthBiasSlopeFactor(float depthBiasSlopeFactor)
	{
		m_depthBiasSlopeFactor = depthBiasSlopeFactor;
	}
	void Renderer::SetOutlineColor(const Float4& outlineColor)
	{
		m_outlineColor = outlineColor;
	}
	void Renderer::SetOutlineThickness(int outlineThickness)
	{
		m_outlineThickness = outlineThickness;
	}



	// Functionality forwarding:
	void Renderer::CollectGarbage()
	{
		if (m_pCompute)
			m_pCompute->GetAsyncCompute()->CollectFinishedSessions();
		GarbageCollector::CollectGarbage();
	}
	void Renderer::WaitDeviceIdle()
	{
		Context::WaitDeviceIdle();
	}
	void Renderer::WaitForFrameFinished(uint32_t frameIndex)
	{
		m_pRenderGraph->WaitForFrame(frameIndex);
	}



	// Gpu resource destruction:
	void Renderer::DestroyComputeShader(emberBackendInterface::IComputeShader* pIComputeShader)
	{
	    if (!pIComputeShader)
	        return;

	    ComputeShader* pComputeShader = static_cast<ComputeShader*>(pIComputeShader);
		GarbageCollector::RecordPendingGarbage([pComputeShader]()
	    {
	        if (pComputeShader->HasPendingUse())
	            return false;
	        delete pComputeShader;
	        return true;
	    });
	}


	// Vulkan handle passthrough for API coupling:
	void* Renderer::GetVkInstance() const
	{
		return static_cast<void*>(Context::GetVkInstance());
	}
	void* Renderer::GetVkPhysicalDevice() const
	{
		return static_cast<void*>(Context::GetVkPhysicalDevice());
	}
	void* Renderer::GetVkDevice() const
	{
		return static_cast<void*>(Context::GetVkDevice());
	}
	void* Renderer::GetPresentVkRenderPass() const
	{
		return static_cast<void*>(RenderPassManager::GetPresentRenderPass()->GetVkRenderPass());
	}
	void* Renderer::GetGraphicsVkQueue() const
	{
		return static_cast<void*>(Context::GetLogicalDevice()->GetGraphicsQueue().queue);
	}
	void* Renderer::GetColorSampler() const
	{
		return static_cast<void*>(DefaultGpuResources::GetColorSampler()->GetVkSampler());
	}
	uint32_t Renderer::GetGraphicsVkQueueFamilyIndex() const
	{
		return Context::GetLogicalDevice()->GetGraphicsQueue().familyIndex;
	}
	uint32_t Renderer::GetSwapchainImageCount() const
	{
		return Context::GetSwapchain()->GetImageCount();
	}
	uint32_t Renderer::GetFramesInFlight() const
	{
		return Context::GetFramesInFlight();
	}



	// Debugging:
	void Renderer::DumpVmaBufferAllocations() const
	{
		Context::GetAllocationTracker()->DumpVmaBufferAllocations();
	}
	void Renderer::DumpVmaImageAllocations() const
	{
		Context::GetAllocationTracker()->DumpVmaImageAllocations();
	}



	// Backend only:
	void Renderer::QueueMeshForUpdate(vulkanRendererBackend::Mesh* pMesh)
	{
		// Prevent double-adding:
		for (FrameRenderData& frameRenderData : m_frameRenderData)
		{
			std::vector<Mesh*>& meshUpdates = frameRenderData.meshUpdates;
			if (std::find(meshUpdates.begin(), meshUpdates.end(), pMesh) == meshUpdates.end())
				meshUpdates.push_back(pMesh);
		}
	}
	void Renderer::RemoveQueuedMeshUpdate(vulkanRendererBackend::Mesh* pMesh)
	{
		for (FrameRenderData& frameRenderData : m_frameRenderData)
			frameRenderData.meshUpdates.erase(std::remove(frameRenderData.meshUpdates.begin(), frameRenderData.meshUpdates.end(), pMesh), frameRenderData.meshUpdates.end());
	}
	void Renderer::ReplaceQueuedMeshUpdate(vulkanRendererBackend::Mesh* pOldMesh, vulkanRendererBackend::Mesh* pNewMesh)
	{
		for (FrameRenderData& frameRenderData : m_frameRenderData)
			for (Mesh*& pMesh : frameRenderData.meshUpdates)
				if (pMesh == pOldMesh)
					pMesh = pNewMesh;
	}
	std::array<VkDescriptorSet, 3>& Renderer::GetStaticDescriptorSets(uint32_t frameIndex)
	{
		return m_staticDescriptorSets[frameIndex];
	}



	// Private methods:
	// Reset render state:
	void Renderer::ResetFrameCalls()
	{
		// Compute calls:
		m_pCompute->DiscardPendingCalls();
		// Light data:
		m_previousDirectionalLightsCount = m_directionalLightsCount;
		m_previousPositionalLightsCount = m_positionalLightsCount;
		std::copy_n(m_directionalLights.begin(), m_directionalLightsCount, m_previousDirectionalLights.begin());
		std::copy_n(m_positionalLights.begin(), m_positionalLightsCount, m_previousPositionalLights.begin());
		m_directionalLightsCount = 0;
		m_positionalLightsCount = 0;
		// Draw calls:
		m_frameRenderData[m_frameIndex].Reset();;
	}



	// Other:
	void Renderer::CreateSceneTextures(uint32_t renderWidth, uint32_t renderHeight)
	{
		const uint32_t framesInFlight = Context::GetFramesInFlight();
		m_pSceneColorTexturePair = std::make_unique<SceneColorTexture2dPair>(renderWidth, renderHeight, framesInFlight);
		m_pSceneDepthTextures.reserve(framesInFlight);
		m_pExpandedOutlineMaskTextures.reserve(framesInFlight);
		m_pHorizontalExpandedOutlineMaskTextures.reserve(framesInFlight);
		for (uint32_t frameIndex = 0; frameIndex < framesInFlight; frameIndex++)
		{
			m_pSceneDepthTextures.push_back(std::make_unique<DepthTexture2d>(deferredRenderingContract::depthFormat, renderWidth, renderHeight));
			m_pSceneDepthTextures[frameIndex]->SetDebugName("SceneDepthTexture_Frame" + std::to_string(frameIndex));
			m_pExpandedOutlineMaskTextures.push_back(std::make_unique<StorageTexture2d>(VK_FORMAT_R8_UNORM, renderWidth, renderHeight));
			m_pExpandedOutlineMaskTextures[frameIndex]->SetDebugName("StorageTexture_ExpandedOutlineMask_Frame" + std::to_string(frameIndex));
			m_pHorizontalExpandedOutlineMaskTextures.push_back(std::make_unique<StorageTexture2d>(VK_FORMAT_R8_UNORM, renderWidth, renderHeight));
			m_pHorizontalExpandedOutlineMaskTextures[frameIndex]->SetDebugName("StorageTexture_HorizontalExpandedOutlineMask_Frame" + std::to_string(frameIndex));

			VkPipelineStageFlags2 srcStage = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
			VkPipelineStageFlags2 dstStage = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
			AccessMask srcAccessMask = AccessMasks::TopOfPipe::none;
			AccessMask dstAccessMask = AccessMasks::BottomOfPipe::none;
			m_pSceneDepthTextures[frameIndex]->GetVmaImage()->TransitionLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, srcStage, dstStage, srcAccessMask, dstAccessMask);
		}
	}
	void Renderer::RebuildSwapchain()
	{
		// Recreate swapchain:
		Context::RebuildSwapchain();	// calls WaitDeviceIdle().

		// Return compute shaders callDescriptorSetBindings and decrement their usage for all frames in flight:
		m_pCompute->RetireAllFrames();

		// Recreate renderpasses:
		RenderPassManager::RecreateRenderPasses();

		// Recreate synchronization objects:
		m_pRenderGraph->RecreateSyncObjects();
	}
	bool Renderer::AcquireImage()
	{
		if (m_pIWindow->GetIsResizing())
		{
			m_pIWindow->ResetIsResizing();
			m_rebuildSwapchain = true;
			return false;
		}

		VkResult result = m_pRenderGraph->AcquireImage(m_frameIndex, m_imageIndex);

		switch (result)
		{
		case VK_SUCCESS:
			return true;
		case VK_SUBOPTIMAL_KHR:
			m_rebuildSwapchain = true;
			return true;
		case VK_ERROR_OUT_OF_DATE_KHR:
			m_rebuildSwapchain = true;
			return false;
		default:
			LOG_CRITICAL("Renderer::AcquireImage() failed. Vulkan error: {}", std::to_string(result));
			std::abort();
		}
	}
	void Renderer::SortDrawCallPointers()
	{
		m_frameRenderData[m_frameIndex].SortDrawCalls(m_activeCamera);
	}
	void Renderer::QueueRendererOwnedComputeShaders()
	{
		// Outline mask:
		MidRender* pMidRenderCompute = m_pCompute->GetMidRenderCompute();
		PostRender* pPostRenderCompute = m_pCompute->GetPostRenderCompute();
		if (!m_frameRenderData[Context::GetFrameIndex()].outlineDrawCalls.empty())
		{
			// Masks:
			RenderTexture2d* pInputMask = RenderPassManager::GetOutlineRenderPass()->GetRenderTexture(m_frameIndex);
			StorageTexture2d* pHorizontalExpandedMask = m_pHorizontalExpandedOutlineMaskTextures[m_frameIndex].get();
			StorageTexture2d* pExpandedMask = m_pExpandedOutlineMaskTextures[m_frameIndex].get();

			// Expand mask horizontally (midRenderCompute):
			Uint3 threadCount = { pInputMask->GetWidth(), pInputMask->GetHeight(), 1 };
			ComputeShader* pHorizontalExpansionComputeShader = DefaultGpuResources::GetOutlineHorizontalMaskExpansionComputeShader();
			pHorizontalExpansionComputeShader->GetDescriptorSetBinding()->SetInt("OutlineProperties", "outlineRadius", m_outlineThickness);
			DescriptorSetBinding* pHorizontalExpansionCallDescriptorSetBinding = static_cast<DescriptorSetBinding*>(pMidRenderCompute->RecordComputeShader(pHorizontalExpansionComputeShader, threadCount));
			if (!pHorizontalExpansionCallDescriptorSetBinding)
				throw std::runtime_error("Renderer::RenderFrame(...) failed. Could not record the horizontal outline mask expansion compute shader.");
			pHorizontalExpansionCallDescriptorSetBinding->SetTexture("inputMask", pInputMask);
			pHorizontalExpansionCallDescriptorSetBinding->SetTexture("outputMask", pHorizontalExpandedMask);

			pMidRenderCompute->RecordBarrier(emberBackendInterface::ComputeBarrierFlag::storageWrite, emberBackendInterface::ComputeBarrierFlag::storageRead);

			// Expand mask vertically and remove the original mask (midRenderCompute):
			ComputeShader* pVerticalExpansionComputeShader = DefaultGpuResources::GetOutlineVerticalMaskExpansionComputeShader();
			pVerticalExpansionComputeShader->GetDescriptorSetBinding()->SetInt("OutlineProperties", "outlineRadius", m_outlineThickness);
			DescriptorSetBinding* pVerticalExpansionCallDescriptorSetBinding = static_cast<DescriptorSetBinding*>(pMidRenderCompute->RecordComputeShader(pVerticalExpansionComputeShader, threadCount));
			if (!pVerticalExpansionCallDescriptorSetBinding)
				throw std::runtime_error("Renderer::RenderFrame(...) failed. Could not record the vertical outline mask expansion compute shader.");
			pVerticalExpansionCallDescriptorSetBinding->SetTexture("inputMask", pHorizontalExpandedMask);
			pVerticalExpansionCallDescriptorSetBinding->SetTexture("originalMask", pInputMask);
			pVerticalExpansionCallDescriptorSetBinding->SetTexture("outputMask", pExpandedMask);

			// Composite outline into render texture (postRenderCompute):
			ComputeShader* pOutlineCompositeComputeShader = DefaultGpuResources::GetOutlineCompositeComputeShader();
			pOutlineCompositeComputeShader->GetDescriptorSetBinding()->SetFloat4("OutlineProperties", "outlineColor", m_outlineColor);
			DescriptorSetBinding* pCompositeCallDescriptorSetBinding = static_cast<DescriptorSetBinding*>(pPostRenderCompute->RecordPostProcessingShader(pOutlineCompositeComputeShader));
			if (!pCompositeCallDescriptorSetBinding)
				throw std::runtime_error("Renderer::RenderFrame(...) failed. Could not record the outline composite compute shader.");
			pCompositeCallDescriptorSetBinding->SetTexture("outlineMask", pExpandedMask);
		}

		// Renderer uses linear color space, apply gamma correction is always the final post-render operation:
		if (pPostRenderCompute->RecordPostProcessingShader(DefaultGpuResources::GetGammaCorrectionComputeShader()) == nullptr)
			throw std::runtime_error("Renderer::RenderFrame(...) failed. Could not record the gamma correction compute shader.");
	}
	void Renderer::UpdateShaderData()
	{
		// Scene descriptor set:
		SceneDescriptorSetLayout::SetLightData(m_directionalLights, m_directionalLightsCount, m_positionalLights, m_positionalLightsCount);
		SceneDescriptorSetLayout::UpdateShaderData(m_frameIndex);

		// Frame descriptor set:
		FrameDescriptorSetLayout::SetCameraData(Float4(m_activeCamera.position, 1.0f), m_activeCamera.viewMatrix, m_activeCamera.projectionMatrix);
		FrameDescriptorSetLayout::UpdateShaderData(m_frameIndex);

		// Gizmo calls:
		for (GizmoDrawCall& drawCall : m_frameRenderData[m_frameIndex].gizmoDrawCalls)
		{
			drawCall.UpdateModelData();
			drawCall.pMaterial->GetDescriptorSetBinding()->UpdateShaderData(m_frameIndex);
			drawCall.descriptorSetBindingHandle.Get()->UpdateShaderData(m_frameIndex);
		}

		// Outline calls:
		if (!m_frameRenderData[Context::GetFrameIndex()].outlineDrawCalls.empty())
			DefaultGpuResources::GetDefaultOutlineMaterial()->GetDescriptorSetBinding()->UpdateShaderData(m_frameIndex);
		for (OutlineDrawCall& drawCall : m_frameRenderData[Context::GetFrameIndex()].outlineDrawCalls)
		{
			drawCall.UpdateModelData();
			drawCall.descriptorSetBindingHandle.Get()->UpdateShaderData(m_frameIndex);
		}

		// Shadow calls:
		for (ShadowDrawCall& drawCall : m_frameRenderData[Context::GetFrameIndex()].shadowDrawCalls)
		{
			drawCall.UpdateModelData();
			drawCall.pMaterial->GetDescriptorSetBinding()->UpdateShaderData(m_frameIndex);
			drawCall.descriptorSetBindingHandle.Get()->UpdateShaderData(m_frameIndex);
		}

		// Deferred calls:
		for (DeferredDrawCall& drawCall : m_frameRenderData[Context::GetFrameIndex()].deferredDrawCalls)
		{
			drawCall.UpdateModelData();
			drawCall.pMaterial->GetDescriptorSetBinding()->UpdateShaderData(m_frameIndex);
			drawCall.descriptorSetBindingHandle.Get()->UpdateShaderData(m_frameIndex);
		}

		// Deferred lighting:
		{
			DeferredLightingRenderPass* pDeferredLightingRenderPass = RenderPassManager::GetDeferredLightingRenderPass();
			DescriptorSetBinding* pDeferredLightingDescriptorSetBinding = DefaultGpuResources::GetDefaultDeferredLightingMaterial()->GetDescriptorSetBinding();
			pDeferredLightingDescriptorSetBinding->SetTexture("gbufferAlbedo", pDeferredLightingRenderPass->GetAlbedoTexture(m_frameIndex));
			pDeferredLightingDescriptorSetBinding->SetTexture("gbufferNormal", pDeferredLightingRenderPass->GetNormalTexture(m_frameIndex));
			pDeferredLightingDescriptorSetBinding->SetTexture("gbufferSurfaceProperties", pDeferredLightingRenderPass->GetSurfacePropertiesTexture(m_frameIndex));
			pDeferredLightingDescriptorSetBinding->SetTexture("gbufferDepth", pDeferredLightingRenderPass->GetDepthTexture(m_frameIndex));
			pDeferredLightingDescriptorSetBinding->UpdateShaderData(m_frameIndex);
		}

		// Forward calls:
		for (ForwardDrawCall& drawCall : m_frameRenderData[Context::GetFrameIndex()].forwardDrawCalls)
		{
			drawCall.UpdateModelData();
			drawCall.pMaterial->GetDescriptorSetBinding()->UpdateShaderData(m_frameIndex);
			drawCall.descriptorSetBindingHandle.Get()->UpdateShaderData(m_frameIndex);
		}

		// Compute calls:
		m_pCompute->UpdateShaderData(m_frameIndex, *m_pSceneColorTexturePair);
	}
	bool Renderer::PresentImage()
	{
		VkResult result = m_pRenderGraph->Present(m_imageIndex);
		switch (result)
		{
		case VK_SUCCESS:
		case VK_SUBOPTIMAL_KHR:
			return true;
		case VK_ERROR_OUT_OF_DATE_KHR:
			m_rebuildSwapchain = true;
			return false;
		default:
			LOG_CRITICAL("Renderer::PresentImage() failed. Vulkan error: {}", std::to_string(result));
			std::abort();
		}
	}
}