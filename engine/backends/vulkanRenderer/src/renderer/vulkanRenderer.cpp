#include "vulkanRenderer.h"
#include "descriptorSetMacros.h"
#include "emberMath.h"
#include "iGui.h"
#include "iWindow.h"
#include "logger.h"
#include "parallelThreadPool.h"
#include "profiler.h"
#include "shadowConstants.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanAccessMask.h"
#include "vulkanAllocationTracker.h"
#include "vulkanAsyncCompute.h"
#include "vulkanCommandPool.h"
#include "vulkanCompute.h"
#include "vulkanComputeCall.h"
#include "vulkanComputePushConstant.h"
#include "vulkanComputeShader.h"
#include "vulkanContext.h"
#include "vulkanConvertMaterialRenderState.h"
#include "vulkanConvertTextureFormat.h"
#include "vulkanDefaultGpuResources.h"
#include "vulkanDefaultPushConstant.h"
#include "vulkanDeferredDrawCall.h"
#include "vulkanDeferredGeometryRenderPass.h"
#include "vulkanDeferredLightingRenderPass.h"
#include "vulkanDeferredRenderingContract.h"
#include "vulkanDepthTexture2d.h"
#include "vulkanDepthTexture2dArray.h"
#include "vulkanDescriptorPoolManager.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanFrameDescriptorSetLayout.h"
#include "vulkanForwardDrawCall.h"
#include "vulkanForwardOpaqueRenderPass.h"
#include "vulkanForwardTransparentRenderPass.h"
#include "vulkanGarbageCollector.h"
#include "vulkanGBufferTexture2d.h"
#include "vulkanGizmoDrawCall.h"
#include "vulkanGizmoRenderPass.h"
#include "vulkanGlobalDescriptorSetLayout.h"
#include "vulkanIndexBuffer.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include "vulkanMaterial.h"
#include "vulkanMaterialManager.h"
#include "vulkanMaterialShaderManager.h"
#include "vulkanMesh.h"
#include "vulkanOutlineDrawCall.h"
#include "vulkanOutlineRenderPass.h"
#include "vulkanPipeline.h"
#include "vulkanPoolManager.h"
#include "vulkanPostRenderCompute.h"
#include "vulkanPreRenderCompute.h"
#include "vulkanPresentRenderPass.h"
#include "vulkanRenderCompute.h"
#include "vulkanRenderPassManager.h"
#include "vulkanRenderTexture2d.h"
#include "vulkanSampler.h"
#include "vulkanSampleTexture2d.h"
#include "vulkanSampleTexture3d.h"
#include "vulkanSampleTextureCube.h"
#include "vulkanSceneColorTexture2dPair.h"
#include "vulkanSceneDescriptorSetLayout.h"
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
#include <functional>
#include <stdexcept>
#include <string>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	Renderer::Renderer(const emberCommon::RendererCreateInfo& createInfo, emberBackendInterface::IWindow* pIWindow)
	{
		m_pIWindow = pIWindow;
		m_pendingMeshUpdates.resize(createInfo.framesInFlight); // Prepare one pending mesh update vector per frame in flight.
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

		// Command pools (one per frameInFlight * renderStage):
		m_commandPools.reserve(Context::GetFramesInFlight() * (int)RenderStage::stageCount);
		for (int i = 0; i < Context::GetFramesInFlight() * (int)RenderStage::stageCount; i++)
			m_commandPools.emplace_back(emberTaskSystem::ParallelThreadPool::GetCoreCount(), Context::GetLogicalDevice()->GetGraphicsQueue());

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

		// Debug naming:
		for (int renderStage = 0; renderStage < (int)RenderStage::stageCount; renderStage++)
			for (int frameIndex = 0; frameIndex < Context::GetFramesInFlight(); frameIndex++)
			{
				std::string name = renderStageNames[renderStage];
				name += "_Frame" + std::to_string(frameIndex);
				NAME_VK_OBJECT(GetCommandPool(frameIndex, renderStage).GetPrimaryVkCommandPool(), "CommandPool_Primary_" + name);
				NAME_VK_OBJECT(GetCommandPool(frameIndex, renderStage).GetPrimaryVkCommandBuffer(), "CommandBuffer_Primary_" + name);
				for (int threadIndex = 0; threadIndex < emberTaskSystem::ParallelThreadPool::GetCoreCount(); threadIndex++)
				{
					NAME_VK_OBJECT(GetCommandPool(frameIndex, renderStage).GetSecondaryVkCommandPool(threadIndex), "CommandPool_Secondary_Thread" + std::to_string(threadIndex) + "_" + name);
					NAME_VK_OBJECT(GetCommandPool(frameIndex, renderStage).GetSecondaryVkCommandBuffer(threadIndex), "CommandBuffer_Secondary_Thread" + std::to_string(threadIndex) + "_" + name);
				}
			}

		// Synchronization objects:
		CreateFences();
		CreateSemaphores();

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
		}
	}
	Renderer::~Renderer()
	{
		Context::WaitDeviceIdle();
		DestroySemaphores();
		DestroyFences();
		m_commandPools.clear();
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
			VKA(vkWaitForFences(Context::GetVkDevice(), 1, &m_frameFences[m_frameIndex], VK_TRUE, UINT64_MAX));
		}

		// Return frame compute shaders callDescriptorSetBindings and decrement their usage count of previous submission with this frame index:
		m_pCompute->GetPreRenderCompute()->CompleteComputeCalls(m_frameIndex);
		m_pCompute->GetRenderCompute()->CompleteComputeCalls(m_frameIndex);
		m_pCompute->GetPostRenderCompute()->CompleteComputeCalls(m_frameIndex);

		// Cancel current frame on failed acquisition:
		if (!AcquireImage())
		{
			ResetFrameCalls();
			return;
		}

		// Begin next frame:
		VKA(vkResetFences(Context::GetVkDevice(), 1, &m_frameFences[m_frameIndex]));
		ResetCommandPools();
		Context::MarkDeviceBusy();
		m_pSceneColorTexturePair->BeginFrame(m_frameIndex);

		SortDrawCallPointers();
		QueueRendererOwnedComputeShaders();
		UpdateShaderData();

		// Record and submit current frame commands:
		{
			PROFILE_SCOPE("vulkanRendererBackend::Renderer::Record");
			DEBUG_LOG_TRACE("Recording frame {}", m_frameIndex);

			RecordResourceUpdateCommands();
			SubmitResourceUpdateCommands();

			RecordGizmoCommands();
			SubmitGizmoCommands();

			RecordPreRenderComputeCommands();
			SubmitPreRenderComputeCommands();

			RecordOutlineCommands();
			SubmitOutlineCommands();

			RecordRenderComputeCommands();
			SubmitRenderComputeCommands();

			RecordShadowCommands();
			SubmitShadowCommands();

			RecordDeferredGeometryCommands();
			SubmitDeferredGeometryCommands();

			RecordDeferredLightingCommands();
			SubmitDeferredLightingCommands();

			RecordForwardCommands<RenderStage::forwardOpaque>();
			SubmitForwardOpaqueCommands();

			RecordForwardCommands<RenderStage::forwardTransparent>();
			SubmitForwardTransparentCommands();

			//tf::Taskflow taskflow;
			//for (int i = 0; i < emberTaskSystem::TaskSystem::GetCoreCount(); i++)
			//	taskflow.emplace([this] { this->RecordForwardCommandsParallel(); }).name("RecordForwardCommandsParallel" + std::to_string(i));
			//emberTaskSystem::TaskSystem::RunAndWait(taskflow);
			//SubmitForwardCommandsParallel();
			// transition of resources to post render compute missing.

			RecordPostRenderComputeCommands();
			SubmitPostRenderComputeCommands();

			if (!Context::DockSpaceEnabled())
				RecordPresentCommands();
			else
				RecordImGuiPresentCommands();
			SubmitPresentCommands();
		}

		// Commit the current frames compute calls, clearing the computeCall vector for the next frame:
		m_pCompute->GetPreRenderCompute()->CommitComputeCalls(m_frameIndex);
		m_pCompute->GetRenderCompute()->CommitComputeCalls(m_frameIndex);
		m_pCompute->GetPostRenderCompute()->CommitComputeCalls(m_frameIndex);

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
		m_outlineCalls.emplace_back(localToWorldMatrix, static_cast<Mesh*>(pIMesh), descriptorSetBindingHandle, instanceCount);
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
				m_deferredDrawCalls.emplace_back(localToWorldMatrix, static_cast<Mesh*>(pIMesh), pMaterial, descriptorSetBindingHandle, cullMode, receiveShadows, instanceCount);
				return;
			case emberCommon::MaterialPass::forward:
				m_forwardDrawCalls.emplace_back(localToWorldMatrix, static_cast<Mesh*>(pIMesh), pMaterial, descriptorSetBindingHandle, cullMode, receiveShadows, instanceCount);
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
				m_deferredDrawCalls.emplace_back(localToWorldMatrix, static_cast<Mesh*>(pIMesh), pMaterial, descriptorSetBindingHandle, cullMode, receiveShadows, instanceCount);
				return descriptorSetBindingHandle.Get();
			}
			case emberCommon::MaterialPass::forward:
			{
				DescriptorSetBindingHandle descriptorSetBindingHandle = PoolManager::CheckOutCallDescriptorSetBindingHandle(pMaterial->GetShader());
				m_forwardDrawCalls.emplace_back(localToWorldMatrix, static_cast<Mesh*>(pIMesh), pMaterial, descriptorSetBindingHandle, cullMode, receiveShadows, instanceCount);
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
		m_shadowDrawCalls.emplace_back(localToWorldMatrix, static_cast<Mesh*>(pIMesh), pMaterial, descriptorSetBindingHandle, instanceCount);
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
		m_shadowDrawCalls.emplace_back(localToWorldMatrix, static_cast<Mesh*>(pIMesh), pMaterial, descriptorSetBindingHandle, instanceCount);
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
		m_gizmoDrawCalls.emplace_back(localToWorldMatrix, static_cast<Mesh*>(pIMesh), pMaterial, descriptorSetBindingHandle, cullMode, instanceCount);
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
		m_gizmoDrawCalls.emplace_back(localToWorldMatrix, static_cast<Mesh*>(pIMesh), pMaterial, descriptorSetBindingHandle, cullMode, instanceCount);
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
		if (frameIndex >= m_frameFences.size())
		{
			LOG_ERROR("Renderer::IsFrameFinished(...) failed. frameIndex '{}' out of range.", frameIndex);
			return true;
		}

		VkResult result = vkGetFenceStatus(Context::GetVkDevice(), m_frameFences[frameIndex]);
		if (result == VK_SUCCESS)
			return true;
		if (result == VK_NOT_READY)
			return false;
		VKA(result);
		return false;
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
		if (frameIndex >= m_frameFences.size())
		{
			LOG_ERROR("Renderer::WaitForFrameFinished(...) failed. frameIndex '{}' out of range.", frameIndex);
			return;
		}
		VKA(vkWaitForFences(Context::GetVkDevice(), 1, &m_frameFences[frameIndex], VK_TRUE, UINT64_MAX));
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
		for (std::vector<Mesh*>& meshUpdates : m_pendingMeshUpdates)
		{
			if (std::find(meshUpdates.begin(), meshUpdates.end(), pMesh) == meshUpdates.end())
				meshUpdates.push_back(pMesh);
		}
	}
	void Renderer::RemoveQueuedMeshUpdate(vulkanRendererBackend::Mesh* pMesh)
	{
		for (std::vector<Mesh*>& meshUpdates : m_pendingMeshUpdates)
			meshUpdates.erase(std::remove(meshUpdates.begin(), meshUpdates.end(), pMesh), meshUpdates.end());
	}
	void Renderer::ReplaceQueuedMeshUpdate(vulkanRendererBackend::Mesh* pOldMesh, vulkanRendererBackend::Mesh* pNewMesh)
	{
		for (std::vector<Mesh*>& meshUpdates : m_pendingMeshUpdates)
			for (Mesh*& pMesh : meshUpdates)
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
		m_pCompute->GetPreRenderCompute()->ResetComputeCalls();
		m_pCompute->GetRenderCompute()->ResetComputeCalls();
		ResetLights();
		ResetDrawCalls();
		m_pCompute->GetPostRenderCompute()->ResetComputeCalls();
	}
	void Renderer::ResetLights()
	{
		m_previousDirectionalLightsCount = m_directionalLightsCount;
		m_previousPositionalLightsCount = m_positionalLightsCount;
		std::copy_n(m_directionalLights.begin(), m_directionalLightsCount, m_previousDirectionalLights.begin());
		std::copy_n(m_positionalLights.begin(), m_positionalLightsCount, m_previousPositionalLights.begin());
		m_directionalLightsCount = 0;
		m_positionalLightsCount = 0;
	}
	void Renderer::ResetDrawCalls()
	{
		// Return all borrowed descriptor set bindings to the corresponding pool:
		for (GizmoDrawCall& drawCall : m_gizmoDrawCalls)
			PoolManager::ReturnCallDescriptorSetBinding(drawCall.descriptorSetBindingHandle);
		for (OutlineDrawCall& drawCall : m_outlineCalls)
			PoolManager::ReturnCallDescriptorSetBinding(drawCall.descriptorSetBindingHandle);
		for (ShadowDrawCall& drawCall : m_shadowDrawCalls)
			PoolManager::ReturnCallDescriptorSetBinding(drawCall.descriptorSetBindingHandle);
		for (DeferredDrawCall& drawCall : m_deferredDrawCalls)
			PoolManager::ReturnCallDescriptorSetBinding(drawCall.descriptorSetBindingHandle);
		for (ForwardDrawCall& drawCall : m_forwardDrawCalls)
			PoolManager::ReturnCallDescriptorSetBinding(drawCall.descriptorSetBindingHandle);

		// Clear all draw calls for next frame:
		m_gizmoDrawCalls.clear();
		m_sortedGizmoDrawCallPointers.clear();
		m_outlineCalls.clear();
		m_shadowDrawCalls.clear();
		m_deferredDrawCalls.clear();
		m_sortedDeferredDrawCallPointers.clear();
		m_forwardDrawCalls.clear();
		m_sortedForwardOpaqueDrawCallPointers.clear();
		m_sortedForwardTransparentDrawCallPointers.clear();
	}
	void Renderer::ResetCommandPools()
	{
		GetCommandPool(m_frameIndex, RenderStage::resourceUpdate).ResetPools();
		GetCommandPool(m_frameIndex, RenderStage::preRenderCompute).ResetPools();
		GetCommandPool(m_frameIndex, RenderStage::outline).ResetPools();
		GetCommandPool(m_frameIndex, RenderStage::renderCompute).ResetPools();
		GetCommandPool(m_frameIndex, RenderStage::shadow).ResetPools();
		GetCommandPool(m_frameIndex, RenderStage::deferredGeometry).ResetPools();
		GetCommandPool(m_frameIndex, RenderStage::deferredLighting).ResetPools();
		GetCommandPool(m_frameIndex, RenderStage::forwardOpaque).ResetPools();
		GetCommandPool(m_frameIndex, RenderStage::forwardTransparent).ResetPools();
		GetCommandPool(m_frameIndex, RenderStage::gizmo).ResetPools();
		GetCommandPool(m_frameIndex, RenderStage::postRenderCompute).ResetPools();
		GetCommandPool(m_frameIndex, RenderStage::present).ResetPools();
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

		// Return frame compute shaders callDescriptorSetBindings and decrement their usage for all frames in flight:
		m_pCompute->GetPreRenderCompute()->CompleteAllComputeCalls();
		m_pCompute->GetRenderCompute()->CompleteAllComputeCalls();
		m_pCompute->GetPostRenderCompute()->CompleteAllComputeCalls();

		// Recreate renderpasses:
		RenderPassManager::RecreateRenderPasses();

		// Recreate synchronization objects:
		DestroyFences();
		DestroySemaphores();
		CreateFences();
		CreateSemaphores();
	}
	bool Renderer::AcquireImage()
	{
		if (m_pIWindow->GetIsResizing())
		{
			m_pIWindow->ResetIsResizing();
			m_rebuildSwapchain = true;
			return false;
		}

		// Signal acquireSemaphore when done:
		VkResult result = vkAcquireNextImageKHR(Context::GetVkDevice(), Context::GetVkSwapchainKHR(), UINT64_MAX, m_acquireSemaphores[m_frameIndex], VK_NULL_HANDLE, &m_imageIndex);

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
		// Populate sorted draw call pointers vector:
		m_sortedGizmoDrawCallPointers.clear();
		m_sortedGizmoDrawCallPointers.reserve(m_gizmoDrawCalls.size());
		for (GizmoDrawCall& drawCall : m_gizmoDrawCalls)
			m_sortedGizmoDrawCallPointers.push_back(&drawCall);
		m_sortedDeferredDrawCallPointers.clear();
		m_sortedDeferredDrawCallPointers.reserve(m_deferredDrawCalls.size());
		for (DeferredDrawCall& drawCall : m_deferredDrawCalls)
			m_sortedDeferredDrawCallPointers.push_back(&drawCall);
		m_sortedForwardOpaqueDrawCallPointers.clear();
		m_sortedForwardOpaqueDrawCallPointers.reserve(m_forwardDrawCalls.size());
		m_sortedForwardTransparentDrawCallPointers.clear();
		m_sortedForwardTransparentDrawCallPointers.reserve(m_forwardDrawCalls.size());
		for (ForwardDrawCall& drawCall : m_forwardDrawCalls)
		{
			if (drawCall.pMaterial->GetForwardRenderMode() == emberCommon::ForwardRenderMode::transparent)
				m_sortedForwardTransparentDrawCallPointers.push_back(&drawCall);
			else
				m_sortedForwardOpaqueDrawCallPointers.push_back(&drawCall);
		}

		// Ember::ToDo: frustum culling and sorting by dist to camera is missing.
		// Sort gizmo calls by renderQueue first, then handle transparent draw order, then group by vertex layout:
		std::sort(m_sortedGizmoDrawCallPointers.begin(), m_sortedGizmoDrawCallPointers.end(), [this](GizmoDrawCall* drawCallA, GizmoDrawCall* drawCallB)
		{
			int renderQueueA = static_cast<int>(drawCallA->pMaterial->GetRenderQueue());
			int renderQueueB = static_cast<int>(drawCallB->pMaterial->GetRenderQueue());
			if (renderQueueA != renderQueueB)
				return renderQueueA < renderQueueB;

			const bool transparentA = drawCallA->pMaterial->IsTransparent();
			const bool transparentB = drawCallB->pMaterial->IsTransparent();
			if (transparentA && transparentB)
			{
				const Float3 drawPositionA = Float3(drawCallA->localToWorldMatrix * Float4(0.0f, 0.0f, 0.0f, 1.0f));
				const Float3 drawPositionB = Float3(drawCallB->localToWorldMatrix * Float4(0.0f, 0.0f, 0.0f, 1.0f));
				const float distanceA = Float3::DistanceSq(drawPositionA, m_activeCamera.position);
				const float distanceB = Float3::DistanceSq(drawPositionB, m_activeCamera.position);
				if (distanceA != distanceB)
					return distanceA > distanceB;
			}

			auto layoutA = drawCallA->pMesh->GetVertexMemoryLayout();
			auto layoutB = drawCallB->pMesh->GetVertexMemoryLayout();
			if (layoutA != layoutB)
				return layoutA < layoutB;
			return drawCallA < drawCallB;
		});

		// Ember::ToDo: frustum culling is missing (also for shadow draw calls).
		// Sort deferred calls by render queue first, then group by vertex layout and material:
		std::sort(m_sortedDeferredDrawCallPointers.begin(), m_sortedDeferredDrawCallPointers.end(), [](DeferredDrawCall* drawCallA, DeferredDrawCall* drawCallB)
		{
			int renderQueueA = static_cast<int>(drawCallA->pMaterial->GetRenderQueue());
			int renderQueueB = static_cast<int>(drawCallB->pMaterial->GetRenderQueue());
			if (renderQueueA != renderQueueB)
				return renderQueueA < renderQueueB;

			auto layoutA = drawCallA->pMesh->GetVertexMemoryLayout();
			auto layoutB = drawCallB->pMesh->GetVertexMemoryLayout();
			if (layoutA != layoutB)
				return layoutA < layoutB;

			if (drawCallA->pMaterial != drawCallB->pMaterial)
				return std::less<Material*>()(drawCallA->pMaterial, drawCallB->pMaterial);
			return std::less<DeferredDrawCall*>()(drawCallA, drawCallB);
		});

		// Ember::ToDo: frustum culling and sorting by dist to camera is missing (also for shadow draw calls).
		// Sort forward calls by renderQueue first, then handle transparent draw order, then group by vertex layout and material:
		auto compareForwardDrawCalls = [this](ForwardDrawCall* drawCallA, ForwardDrawCall* drawCallB)
		{
			int renderQueueA = static_cast<int>(drawCallA->pMaterial->GetRenderQueue());
			int renderQueueB = static_cast<int>(drawCallB->pMaterial->GetRenderQueue());
			if (renderQueueA != renderQueueB)
				return renderQueueA < renderQueueB;

			const bool transparentA = drawCallA->pMaterial->IsTransparent();
			const bool transparentB = drawCallB->pMaterial->IsTransparent();
			if (transparentA && transparentB)
			{
				const Float3 drawPositionA = Float3(drawCallA->localToWorldMatrix * Float4(0.0f, 0.0f, 0.0f, 1.0f));
				const Float3 drawPositionB = Float3(drawCallB->localToWorldMatrix * Float4(0.0f, 0.0f, 0.0f, 1.0f));
				const float distanceA = Float3::DistanceSq(drawPositionA, m_activeCamera.position);
				const float distanceB = Float3::DistanceSq(drawPositionB, m_activeCamera.position);
				if (distanceA != distanceB)
					return distanceA > distanceB;
			}

			auto layoutA = drawCallA->pMesh->GetVertexMemoryLayout();
			auto layoutB = drawCallB->pMesh->GetVertexMemoryLayout();
			if (layoutA != layoutB)
				return layoutA < layoutB;

			if (drawCallA->pMaterial != drawCallB->pMaterial)
				return std::less<Material*>()(drawCallA->pMaterial, drawCallB->pMaterial);
			return std::less<ForwardDrawCall*>()(drawCallA, drawCallB);
		};
		std::sort(m_sortedForwardOpaqueDrawCallPointers.begin(), m_sortedForwardOpaqueDrawCallPointers.end(), compareForwardDrawCalls);
		std::sort(m_sortedForwardTransparentDrawCallPointers.begin(), m_sortedForwardTransparentDrawCallPointers.end(), compareForwardDrawCalls);
	}
	void Renderer::QueueRendererOwnedComputeShaders()
	{
		// Outline mask:
		Render* pRenderCompute = m_pCompute->GetRenderCompute();
		PostRender* pPostRenderCompute = m_pCompute->GetPostRenderCompute();
		if (!m_outlineCalls.empty())
		{
			// Masks:
			RenderTexture2d* pInputMask = RenderPassManager::GetOutlineRenderPass()->GetRenderTexture(m_frameIndex);
			StorageTexture2d* pHorizontalExpandedMask = m_pHorizontalExpandedOutlineMaskTextures[m_frameIndex].get();
			StorageTexture2d* pExpandedMask = m_pExpandedOutlineMaskTextures[m_frameIndex].get();

			// Expand mask horizontally (renderCompute):
			Uint3 threadCount = { pInputMask->GetWidth(), pInputMask->GetHeight(), 1 };
			ComputeShader* pHorizontalExpansionComputeShader = DefaultGpuResources::GetOutlineHorizontalMaskExpansionComputeShader();
			pHorizontalExpansionComputeShader->GetDescriptorSetBinding()->SetInt("OutlineProperties", "outlineRadius", m_outlineThickness);
			DescriptorSetBinding* pHorizontalExpansionCallDescriptorSetBinding = static_cast<DescriptorSetBinding*>(pRenderCompute->RecordComputeShader(pHorizontalExpansionComputeShader, threadCount));
			if (!pHorizontalExpansionCallDescriptorSetBinding)
				throw std::runtime_error("Renderer::RenderFrame(...) failed. Could not record the horizontal outline mask expansion compute shader.");
			pHorizontalExpansionCallDescriptorSetBinding->SetTexture("inputMask", pInputMask);
			pHorizontalExpansionCallDescriptorSetBinding->SetTexture("outputMask", pHorizontalExpandedMask);

			pRenderCompute->RecordBarrier(emberBackendInterface::ComputeBarrierFlag::storageWrite, emberBackendInterface::ComputeBarrierFlag::storageRead);

			// Expand mask vertically and remove the original mask (renderCompute):
			ComputeShader* pVerticalExpansionComputeShader = DefaultGpuResources::GetOutlineVerticalMaskExpansionComputeShader();
			pVerticalExpansionComputeShader->GetDescriptorSetBinding()->SetInt("OutlineProperties", "outlineRadius", m_outlineThickness);
			DescriptorSetBinding* pVerticalExpansionCallDescriptorSetBinding = static_cast<DescriptorSetBinding*>(pRenderCompute->RecordComputeShader(pVerticalExpansionComputeShader, threadCount));
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

		// Pre render compute:
		for (ComputeCall& computeCall : m_pCompute->GetPreRenderCompute()->GetComputeCalls())
			if (!computeCall.IsBarrier())
			{
				computeCall.GetComputeShader()->GetDescriptorSetBinding()->UpdateShaderData(m_frameIndex);
				computeCall.callDescriptorSetBindingHandle.Get()->UpdateShaderData(m_frameIndex);
			}

		// Render compute:
		for (ComputeCall& computeCall : m_pCompute->GetRenderCompute()->GetComputeCalls())
			if (!computeCall.IsBarrier())
			{
				computeCall.GetComputeShader()->GetDescriptorSetBinding()->UpdateShaderData(m_frameIndex);
				computeCall.callDescriptorSetBindingHandle.Get()->UpdateShaderData(m_frameIndex);
			}

		// Gizmo calls:
		for (GizmoDrawCall& drawCall : m_gizmoDrawCalls)
		{
			drawCall.UpdateModelData();
			drawCall.pMaterial->GetDescriptorSetBinding()->UpdateShaderData(m_frameIndex);
			drawCall.descriptorSetBindingHandle.Get()->UpdateShaderData(m_frameIndex);
		}

		// Outline calls:
		if (!m_outlineCalls.empty())
			DefaultGpuResources::GetDefaultOutlineMaterial()->GetDescriptorSetBinding()->UpdateShaderData(m_frameIndex);
		for (OutlineDrawCall& drawCall : m_outlineCalls)
		{
			drawCall.UpdateModelData();
			drawCall.descriptorSetBindingHandle.Get()->UpdateShaderData(m_frameIndex);
		}

		// Shadow calls:
		for (ShadowDrawCall& drawCall : m_shadowDrawCalls)
		{
			drawCall.UpdateModelData();
			drawCall.pMaterial->GetDescriptorSetBinding()->UpdateShaderData(m_frameIndex);
			drawCall.descriptorSetBindingHandle.Get()->UpdateShaderData(m_frameIndex);
		}

		// Deferred calls:
		for (DeferredDrawCall& drawCall : m_deferredDrawCalls)
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
			pDeferredLightingDescriptorSetBinding->SetTexture("gbufferMaterial", pDeferredLightingRenderPass->GetMaterialTexture(m_frameIndex));
			pDeferredLightingDescriptorSetBinding->SetTexture("gbufferDepth", pDeferredLightingRenderPass->GetDepthTexture(m_frameIndex));
			pDeferredLightingDescriptorSetBinding->UpdateShaderData(m_frameIndex);
		}

		// Forward calls:
		for (ForwardDrawCall& drawCall : m_forwardDrawCalls)
		{
			drawCall.UpdateModelData();
			drawCall.pMaterial->GetDescriptorSetBinding()->UpdateShaderData(m_frameIndex);
			drawCall.descriptorSetBindingHandle.Get()->UpdateShaderData(m_frameIndex);
		}

		// Post render compute:
		PostRender* pPostRenderCompute = m_pCompute->GetPostRenderCompute();
		Uint3 postRenderThreadCount = { m_pSceneColorTexturePair->GetWidth(), m_pSceneColorTexturePair->GetHeight(), 1 };

		// Configure all post-render compute calls and advance the scene-color ping-pong chain:
		for (ComputeCall& computeCall : pPostRenderCompute->GetComputeCalls())
		{
			computeCall.threadCount = postRenderThreadCount;
			switch (computeCall.postProcessingMode)
			{
			case PostProcessingMode::none:
				break;
			case PostProcessingMode::inPlace:
				computeCall.callDescriptorSetBindingHandle.Get()->SetTexture("inOutImage", m_pSceneColorTexturePair->GetCurrentTexture(m_frameIndex));
				break;
			case PostProcessingMode::outOfPlace:
				computeCall.callDescriptorSetBindingHandle.Get()->SetTexture("inputImage", m_pSceneColorTexturePair->GetCurrentTexture(m_frameIndex));
				computeCall.callDescriptorSetBindingHandle.Get()->SetTexture("outputImage", m_pSceneColorTexturePair->GetNextTexture(m_frameIndex));
				m_pSceneColorTexturePair->Swap(m_frameIndex);
				break;
			}
			computeCall.GetComputeShader()->GetDescriptorSetBinding()->UpdateShaderData(m_frameIndex);
			computeCall.callDescriptorSetBindingHandle.Get()->UpdateShaderData(m_frameIndex);
		}

	}



	// Record commands:
	void Renderer::RecordResourceUpdateCommands()
	{
		PROFILE_FUNCTION();

		// Prepare meshes to update:
		std::vector<Mesh*>& meshUpdates = m_pendingMeshUpdates[m_frameIndex];

		// Prepare command recording:
		CommandPool& commandPool = GetCommandPool(m_frameIndex, RenderStage::resourceUpdate);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		// Record resource update commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			for (Mesh* mesh : meshUpdates)
				mesh->RecordUpdateCommand(commandBuffer, m_frameIndex);
		}
		VKA(vkEndCommandBuffer(commandBuffer));

		// Clear only mesh updates of current frame:
		meshUpdates.clear();
	}
	void Renderer::RecordGizmoCommands()
	{
		PROFILE_FUNCTION();

		// Prepare command recording:
		CommandPool& commandPool = GetCommandPool(m_frameIndex, RenderStage::gizmo);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		GizmoRenderPass* pGizmoRenderPass = RenderPassManager::GetGizmoRenderPass();

		// Record gizmo commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			// Viewport and scissor:
			VkViewport viewport = {};
			viewport.width = pGizmoRenderPass->GetRenderTexture(m_frameIndex)->GetWidth();
			viewport.height = pGizmoRenderPass->GetRenderTexture(m_frameIndex)->GetHeight();
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			VkRect2D scissor = {};
			scissor.extent.width = viewport.width;
			scissor.extent.height = viewport.height;
			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

			// Render pass info:
			std::array<VkClearValue, 2> clearValues;
			clearValues[0].color = { 0.0f, 0.0f, 0.0f, 0.0f };
			clearValues[1].depthStencil = { 1.0f, 0 };
			VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
			renderPassBeginInfo.renderPass = pGizmoRenderPass->GetVkRenderPass();
			renderPassBeginInfo.framebuffer = pGizmoRenderPass->GetFramebuffer(m_frameIndex);
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent.width = viewport.width;
			renderPassBeginInfo.renderArea.extent.height = viewport.height;
			renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassBeginInfo.pClearValues = clearValues.data();

			// Begin render pass:
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			if (m_sortedGizmoDrawCallPointers.size() > 0)
			{
				// Pipeline:
				VkPipeline pipeline = VK_NULL_HANDLE;
				VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
				VkDescriptorSet shaderDescriptorSet = VK_NULL_HANDLE;
				bool staticDescriptorSetsBound = false;

				// Draw calls:
				for (GizmoDrawCall* drawCall : m_sortedGizmoDrawCallPointers)
				{
					// Pipeline swap:
					Material* pGizmoMaterial = drawCall->pMaterial;
					VkPipeline newPipeline = pGizmoMaterial->GetPipeline<RenderStage::gizmo>(drawCall->pMesh)->GetVkPipeline();
					bool pipelineLayoutChanged = false;
					if (pipeline != newPipeline)
					{
						// Bind Pipeline:
						pipeline = newPipeline;
						vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

						// Pipeline layout swap:
						VkPipelineLayout newPipelineLayout = pGizmoMaterial->GetVkPipelineLayout();
						pipelineLayoutChanged = pipelineLayout != newPipelineLayout;
						if (pipelineLayoutChanged)
						{
							pipelineLayout = newPipelineLayout;

							// Bind static descriptor sets:
							if (!staticDescriptorSetsBound)
							{
								vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 3, m_staticDescriptorSets[m_frameIndex].data(), 0, nullptr);
								staticDescriptorSetsBound = true;
							}
						}
					}

					// Bind per shader descriptor set:
					VkDescriptorSet newShaderDescriptorSet = pGizmoMaterial->GetDescriptorSetBinding()->GetVkDescriptorSet(m_frameIndex);
					if (newShaderDescriptorSet != VK_NULL_HANDLE && (pipelineLayoutChanged || shaderDescriptorSet != newShaderDescriptorSet))
					{
						shaderDescriptorSet = newShaderDescriptorSet;
						vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, SHADER_SET_INDEX, 1, &shaderDescriptorSet, 0, nullptr);
					}

					// Push constant:
					DefaultPushConstant pushConstant(0, drawCall->instanceCount, false, m_time, m_deltaTime);
					vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(DefaultPushConstant), &pushConstant);

					// Cull mode:
					vkCmdSetCullMode(commandBuffer, CullModeCommonToVulkan(drawCall->cullMode));

					// Bind per draw call descriptor set:
					if (VkDescriptorSet vkDescriptorSet = drawCall->descriptorSetBindingHandle.Get()->GetVkDescriptorSet(m_frameIndex); vkDescriptorSet != VK_NULL_HANDLE)
						vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, CALL_SET_INDEX, 1, &vkDescriptorSet, 0, nullptr);

					// Bind mesh data:
					vkCmdBindVertexBuffers(commandBuffer, 0, drawCall->pMesh->GetVertexBindingCount(), drawCall->pMesh->GetVkBuffers(), drawCall->pMesh->GetOffsets());
					vkCmdBindIndexBuffer(commandBuffer, drawCall->pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, drawCall->pMesh->GetVkIndexType());

					// Dispatch:
					vkCmdDrawIndexed(commandBuffer, drawCall->pMesh->GetIndexCount(), std::max(drawCall->instanceCount, (uint32_t)1), 0, 0, 0);
					DEBUG_LOG_TRACE("Gizmo draw call, mesh = {}, material = {}", drawCall->pMesh->GetName(), pGizmoMaterial->GetDebugName());
				}
			}
			vkCmdEndRenderPass(commandBuffer);
		}
		VKA(vkEndCommandBuffer(commandBuffer));

		// Gizmo render pass's color resolve finalLayout is VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL. Reflect this in the image layout:
		pGizmoRenderPass->GetRenderTexture(m_frameIndex)->GetVmaImage()->SetLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}
    void Renderer::RecordPreRenderComputeCommands()
	{
		PROFILE_FUNCTION();

		// Prepare command recording:
		CommandPool& commandPool = GetCommandPool(m_frameIndex, RenderStage::preRenderCompute);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		// Record pre render compute commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			std::vector<ComputeCall>& computeCalls = m_pCompute->GetPreRenderCompute()->GetComputeCalls();
			if (!computeCalls.empty())
			{
				// Pipeline:
				VkPipeline pipeline = VK_NULL_HANDLE;
				VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
				bool staticDescriptorSetsBound = false;

				for (size_t computeCallIndex = 0; computeCallIndex < computeCalls.size(); computeCallIndex++)
				{
					ComputeCall* computeCall = &computeCalls[computeCallIndex];

					// Compute call is a barrier:
					if (computeCall->IsBarrier())
					{
						VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
						memoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
						memoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
						memoryBarrier.srcAccessMask = computeCall->srcAccessMask;
						memoryBarrier.dstAccessMask = computeCall->dstAccessMask;

						VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
						dependencyInfo.memoryBarrierCount = 1;
						dependencyInfo.pMemoryBarriers = &memoryBarrier;

						vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
						DEBUG_LOG_TRACE("Pre Render Compute Barrier, call = {}", computeCallIndex);
					}
					// Compute call is a dispatch:
					else
					{
						// Pipeline change:
						ComputeShader* pComputeShader = computeCall->GetComputeShader();
						VkPipeline newPipeline = pComputeShader->GetPipeline()->GetVkPipeline();
						if (pipeline != newPipeline)
						{
							// Bind Pipeline:
							pipeline = newPipeline;
							pipelineLayout = pComputeShader->GetVkPipelineLayout();
							vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);

							// Bind static descriptor sets:
							if (!staticDescriptorSetsBound)
							{
								vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 3, m_staticDescriptorSets[m_frameIndex].data(), 0, nullptr);
								staticDescriptorSetsBound = true;
							}

							// Bind per shader descriptor set:
							if (VkDescriptorSet vkDescriptorSet = pComputeShader->GetDescriptorSetBinding()->GetVkDescriptorSet(m_frameIndex); vkDescriptorSet != VK_NULL_HANDLE)
								vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, SHADER_SET_INDEX, 1, &vkDescriptorSet, 0, nullptr);
						}

						// Bind per compute call descriptor set:
						if (VkDescriptorSet vkDescriptorSet = computeCall->callDescriptorSetBindingHandle.Get()->GetVkDescriptorSet(m_frameIndex); vkDescriptorSet != VK_NULL_HANDLE)
							vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, CALL_SET_INDEX, 1, &vkDescriptorSet, 0, nullptr);

						// Push constant:
						ComputePushConstant pushConstant(computeCall->threadCount, m_time, m_deltaTime);
						vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstant), &pushConstant);

						// Group counts:
						Uint3 blockSize = pComputeShader->GetBlockSize();
						uint32_t groupCountX = (computeCall->threadCount[0] + blockSize[0] - 1) / blockSize[0];
						uint32_t groupCountY = (computeCall->threadCount[1] + blockSize[1] - 1) / blockSize[1];
						uint32_t groupCountZ = (computeCall->threadCount[2] + blockSize[2] - 1) / blockSize[2];

						// Dispatch:
						vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
						DEBUG_LOG_TRACE("Pre Render Compute Shader {}, call = {}", pComputeShader->GetDebugName(), computeCallIndex);
					}
				}

				// Release memory from pre render compute shaders to vertex shaders:
				{
					VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
					memoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
					memoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
					memoryBarrier.srcAccessMask = AccessMasks::ComputeShader::shaderWrite;
					memoryBarrier.dstAccessMask = AccessMasks::VertexShader::shaderRead;

					VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
					dependencyInfo.memoryBarrierCount = 1;
					dependencyInfo.pMemoryBarriers = &memoryBarrier;

					vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
					DEBUG_LOG_TRACE("Memory Barrier: pre compute to vertex");
				}
			}
		}
		VKA(vkEndCommandBuffer(commandBuffer));
	}
    void Renderer::RecordOutlineCommands()
    {
		PROFILE_FUNCTION();

		// Prepare command recording:
		CommandPool& commandPool = GetCommandPool(m_frameIndex, RenderStage::outline);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		OutlineRenderPass* pOutlineRenderPass = RenderPassManager::GetOutlineRenderPass();

		// Record outline commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			// Viewport and scissor:
			VkViewport viewport = {};
			viewport.width = pOutlineRenderPass->GetRenderTexture(m_frameIndex)->GetWidth();
			viewport.height = pOutlineRenderPass->GetRenderTexture(m_frameIndex)->GetHeight();
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			VkRect2D scissor = {};
			scissor.extent.width = viewport.width;
			scissor.extent.height = viewport.height;
			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

			// Render pass info:
			VkClearValue clearValues = {};
			clearValues.color = { 0.0f, 0.0f, 0.0f, 0.0f };
			VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
			renderPassBeginInfo.renderPass = pOutlineRenderPass->GetVkRenderPass();
			renderPassBeginInfo.framebuffer = pOutlineRenderPass->GetFramebuffer(m_frameIndex);
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent.width = viewport.width;
			renderPassBeginInfo.renderArea.extent.height = viewport.height;
			renderPassBeginInfo.clearValueCount = 1;
			renderPassBeginInfo.pClearValues = &clearValues;

			// Begin render pass:
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			if (m_outlineCalls.size() > 0)
			{
				// Pipeline:
				VkPipeline pipeline = VK_NULL_HANDLE;
				VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
				VkDescriptorSet shaderDescriptorSet = VK_NULL_HANDLE;
				bool staticDescriptorSetsBound = false;

				// Draw calls:
				Material* pOutlineMaterial = DefaultGpuResources::GetDefaultOutlineMaterial();
				for (const OutlineDrawCall& drawCall : m_outlineCalls)
				{
					// Pipeline swap:
					VkPipeline newPipeline = pOutlineMaterial->GetPipeline<RenderStage::outline>(drawCall.pMesh)->GetVkPipeline();
					bool pipelineLayoutChanged = false;
					if (pipeline != newPipeline)
					{
						// Bind Pipeline:
						pipeline = newPipeline;
						vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

						// Pipeline layout swap:
						VkPipelineLayout newPipelineLayout = pOutlineMaterial->GetVkPipelineLayout();
						pipelineLayoutChanged = pipelineLayout != newPipelineLayout;
						if (pipelineLayoutChanged)
						{
							pipelineLayout = newPipelineLayout;

							// Bind static descriptor sets:
							if (!staticDescriptorSetsBound)
							{
								vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 3, m_staticDescriptorSets[m_frameIndex].data(), 0, nullptr);
								staticDescriptorSetsBound = true;
							}
						}
					}

					// Bind per shader descriptor set:
					VkDescriptorSet newShaderDescriptorSet = pOutlineMaterial->GetDescriptorSetBinding()->GetVkDescriptorSet(m_frameIndex);
					if (newShaderDescriptorSet != VK_NULL_HANDLE && (pipelineLayoutChanged || shaderDescriptorSet != newShaderDescriptorSet))
					{
						shaderDescriptorSet = newShaderDescriptorSet;
						vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, SHADER_SET_INDEX, 1, &shaderDescriptorSet, 0, nullptr);
					}

					// Push constant:
					DefaultPushConstant pushConstant(0, drawCall.instanceCount, false, m_time, m_deltaTime);
					vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(DefaultPushConstant), &pushConstant);

					// Bind per draw call descriptor set:
					if (VkDescriptorSet vkDescriptorSet = drawCall.descriptorSetBindingHandle.Get()->GetVkDescriptorSet(m_frameIndex); vkDescriptorSet != VK_NULL_HANDLE)
						vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, CALL_SET_INDEX, 1, &vkDescriptorSet, 0, nullptr);

					// Bind mesh data:
					vkCmdBindVertexBuffers(commandBuffer, 0, drawCall.pMesh->GetVertexBindingCount(), drawCall.pMesh->GetVkBuffers(), drawCall.pMesh->GetOffsets());
					vkCmdBindIndexBuffer(commandBuffer, drawCall.pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, drawCall.pMesh->GetVkIndexType());

					// Dispatch:
					vkCmdDrawIndexed(commandBuffer, drawCall.pMesh->GetIndexCount(), std::max(drawCall.instanceCount, (uint32_t)1), 0, 0, 0);
					DEBUG_LOG_TRACE("Outline draw call, mesh = {}, material = {}", drawCall.pMesh->GetName(), pOutlineMaterial->GetDebugName());
				}
			}
			vkCmdEndRenderPass(commandBuffer);
		}
		VKA(vkEndCommandBuffer(commandBuffer));

		// Outline render pass's color attachment finalLayout is VK_IMAGE_LAYOUT_GENERAL. Reflect this in the image layout:
		pOutlineRenderPass->GetRenderTexture(m_frameIndex)->GetVmaImage()->SetLayout(VK_IMAGE_LAYOUT_GENERAL);
    }
	void Renderer::RecordRenderComputeCommands()
	{
		PROFILE_FUNCTION();

		// Prepare command recording:
		CommandPool& commandPool = GetCommandPool(m_frameIndex, RenderStage::renderCompute);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		// Record render compute commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			std::vector<ComputeCall>& computeCalls = m_pCompute->GetRenderCompute()->GetComputeCalls();
			if (!computeCalls.empty())
			{
				// Pipeline:
				VkPipeline pipeline = VK_NULL_HANDLE;
				VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
				bool staticDescriptorSetsBound = false;

				for (size_t computeCallIndex = 0; computeCallIndex < computeCalls.size(); computeCallIndex++)
				{
					ComputeCall* pComputeCall = &computeCalls[computeCallIndex];

					// Compute call is a barrier:
					if (pComputeCall->IsBarrier())
					{
						VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
						memoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
						memoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
						memoryBarrier.srcAccessMask = pComputeCall->srcAccessMask;
						memoryBarrier.dstAccessMask = pComputeCall->dstAccessMask;

						VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
						dependencyInfo.memoryBarrierCount = 1;
						dependencyInfo.pMemoryBarriers = &memoryBarrier;

						vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
						DEBUG_LOG_TRACE("Render Compute Barrier, call = {}", computeCallIndex);
					}
					else
					{
						// Pipeline change:
						ComputeShader* pComputeShader = pComputeCall->GetComputeShader();
						VkPipeline newPipeline = pComputeShader->GetPipeline()->GetVkPipeline();
						if (pipeline != newPipeline)
						{
							// Bind Pipeline:
							pipeline = newPipeline;
							pipelineLayout = pComputeShader->GetVkPipelineLayout();
							vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
						
							// Bind static descriptor sets:
							if (!staticDescriptorSetsBound)
							{
								vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 3, m_staticDescriptorSets[m_frameIndex].data(), 0, nullptr);
								staticDescriptorSetsBound = true;
							}
						
							// Bind per shader descriptor set:
							if (VkDescriptorSet vkDescriptorSet = pComputeShader->GetDescriptorSetBinding()->GetVkDescriptorSet(m_frameIndex); vkDescriptorSet != VK_NULL_HANDLE)
								vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, SHADER_SET_INDEX, 1, &vkDescriptorSet, 0, nullptr);
						}
					
						// Bind per compute call descriptor set:
						if (VkDescriptorSet vkDescriptorSet = pComputeCall->callDescriptorSetBindingHandle.Get()->GetVkDescriptorSet(m_frameIndex); vkDescriptorSet != VK_NULL_HANDLE)
							vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, CALL_SET_INDEX, 1, &vkDescriptorSet, 0, nullptr);
					
						// Push constant:
						ComputePushConstant pushConstant(pComputeCall->threadCount, m_time, m_deltaTime);
						vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstant), &pushConstant);
					
						// Group counts:
						Uint3 blockSize = pComputeShader->GetBlockSize();
						uint32_t groupCountX = (pComputeCall->threadCount[0] + blockSize[0] - 1) / blockSize[0];
						uint32_t groupCountY = (pComputeCall->threadCount[1] + blockSize[1] - 1) / blockSize[1];
						uint32_t groupCountZ = (pComputeCall->threadCount[2] + blockSize[2] - 1) / blockSize[2];
					
						// Dispatch:
						vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
						DEBUG_LOG_TRACE("Render Compute Shader {}, call = {}", pComputeShader->GetDebugName(), computeCallIndex);
					}
				}
			}
		}
		VKA(vkEndCommandBuffer(commandBuffer));
	}
    void Renderer::RecordShadowCommands()
	{
		PROFILE_FUNCTION();

		// Prepare command recording:
		CommandPool& commandPool = GetCommandPool(m_frameIndex, RenderStage::shadow);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		ShadowRenderPass* pShadowRenderPass = RenderPassManager::GetShadowRenderPass();

		// Record shadow commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			// Render pass info:
			VkClearValue clearValues = {};
			clearValues.depthStencil = { 1.0f, 0 };
			VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
			renderPassBeginInfo.renderPass = pShadowRenderPass->GetVkRenderPass();
			renderPassBeginInfo.framebuffer = pShadowRenderPass->GetFramebuffer(0);
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent = VkExtent2D{ m_shadowMapResolution, m_shadowMapResolution };
			renderPassBeginInfo.clearValueCount = 1;
			renderPassBeginInfo.pClearValues = &clearValues;

			// Begin render pass:
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			if (!m_shadowDrawCalls.empty())
			{
				// Pipeline:
				VkPipeline pipeline = VK_NULL_HANDLE;
				VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
				VkDescriptorSet shaderDescriptorSet = VK_NULL_HANDLE;
				bool staticDescriptorSetsBound = false;

				// Lights:
				const uint32_t shadowLightCount = m_directionalLightsCount + m_positionalLightsCount;
				if (shadowLightCount > 0)
				{
					// Depth bias:
					vkCmdSetDepthBias(commandBuffer, m_depthBiasConstantFactor, m_depthBiasClamp, m_depthBiasSlopeFactor);

					// Draw calls:
					for (ShadowDrawCall& drawCall : m_shadowDrawCalls)
					{
						// Pipeline swap:
						const Material* pShadowMaterial = drawCall.pMaterial;
						VkPipeline newPipeline = pShadowMaterial->GetPipeline<RenderStage::shadow>(drawCall.pMesh)->GetVkPipeline();
						bool pipelineLayoutChanged = false;
						if (pipeline != newPipeline)
						{
							// Bind Pipeline:
							pipeline = newPipeline;
							vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

							// Pipeline layout swap:
							VkPipelineLayout newPipelineLayout = pShadowMaterial->GetVkPipelineLayout();
							pipelineLayoutChanged = pipelineLayout != newPipelineLayout;
							if (pipelineLayoutChanged)
							{
								pipelineLayout = newPipelineLayout;

								// Bind static descriptor sets:
								if (!staticDescriptorSetsBound)
								{
									vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 3, m_staticDescriptorSets[m_frameIndex].data(), 0, nullptr);
									staticDescriptorSetsBound = true;
								}
							}
						}

						// Bind per shader descriptor set:
						VkDescriptorSet newShaderDescriptorSet = pShadowMaterial->GetDescriptorSetBinding()->GetVkDescriptorSet(m_frameIndex);
						if (newShaderDescriptorSet != VK_NULL_HANDLE && (pipelineLayoutChanged || shaderDescriptorSet != newShaderDescriptorSet))
						{
							shaderDescriptorSet = newShaderDescriptorSet;
							vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, SHADER_SET_INDEX, 1, &shaderDescriptorSet, 0, nullptr);
						}

						// Bind per draw call descriptor set:
						if (VkDescriptorSet vkDescriptorSet = drawCall.descriptorSetBindingHandle.Get()->GetVkDescriptorSet(m_frameIndex); vkDescriptorSet != VK_NULL_HANDLE)
							vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, CALL_SET_INDEX, 1, &vkDescriptorSet, 0, nullptr);

						// Bind mesh data:
						vkCmdBindVertexBuffers(commandBuffer, 0, drawCall.pMesh->GetVertexBindingCount(), drawCall.pMesh->GetVkBuffers(), drawCall.pMesh->GetOffsets());
						vkCmdBindIndexBuffer(commandBuffer, drawCall.pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, drawCall.pMesh->GetVkIndexType());

						for (uint32_t shadowMapIndex = 0; shadowMapIndex < shadowLightCount; shadowMapIndex++)
						{
							// Push constant:
							DefaultPushConstant pushConstant(shadowMapIndex, drawCall.instanceCount, false, m_time, m_deltaTime);
							vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(DefaultPushConstant), &pushConstant);

							// Dispatch:
							vkCmdDrawIndexed(commandBuffer, drawCall.pMesh->GetIndexCount(), std::max(drawCall.instanceCount, (uint32_t)1), 0, 0, 0);
							DEBUG_LOG_INFO("Light {}, mesh = {}", shadowMapIndex, drawCall.pMesh->GetName());
						}
					}
				}
			}
			vkCmdEndRenderPass(commandBuffer);
			// The render pass transitions the shadow maps into its final sampled layout -> VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			// Mirror that state in VmaImage's external layout tracking:
			pShadowRenderPass->GetShadowMaps()->GetVmaImage()->SetLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}
		VKA(vkEndCommandBuffer(commandBuffer));
	}
	void Renderer::RecordDeferredGeometryCommands()
	{
		PROFILE_FUNCTION();

		// Prepare command recording:
		CommandPool& commandPool = GetCommandPool(m_frameIndex, RenderStage::deferredGeometry);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		DeferredGeometryRenderPass* pDeferredGeometryRenderPass = RenderPassManager::GetDeferredGeometryRenderPass();

		// Record deferred geometry commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			// Viewport and scissor:
			VkViewport viewport = {};
			viewport.width = pDeferredGeometryRenderPass->GetAlbedoTexture(m_frameIndex)->GetWidth();
			viewport.height = pDeferredGeometryRenderPass->GetAlbedoTexture(m_frameIndex)->GetHeight();
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			VkRect2D scissor = {};
			scissor.extent.width = viewport.width;
			scissor.extent.height = viewport.height;
			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

			// Render pass info:
			std::array<VkClearValue, 4> clearValues{};
			clearValues[0].color = { 0.0f, 0.0f, 0.0f, 0.0f };
			clearValues[1].color = { 0.0f, 0.0f, 0.0f, 0.0f };
			clearValues[2].color = { 0.0f, 0.0f, 0.0f, 0.0f };
			clearValues[3].depthStencil = { 1.0f, 0 };
			VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
			renderPassBeginInfo.renderPass = pDeferredGeometryRenderPass->GetVkRenderPass();
			renderPassBeginInfo.framebuffer = pDeferredGeometryRenderPass->GetFramebuffer(m_frameIndex);
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent.width = viewport.width;
			renderPassBeginInfo.renderArea.extent.height = viewport.height;
			renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassBeginInfo.pClearValues = clearValues.data();

			// Begin render pass:
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			if (!m_sortedDeferredDrawCallPointers.empty())
			{
				// Pipeline:
				VkPipeline pipeline = VK_NULL_HANDLE;
				VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
				VkDescriptorSet shaderDescriptorSet = VK_NULL_HANDLE;
				bool staticDescriptorSetsBound = false;

				// Draw calls:
				for (DeferredDrawCall* drawCall : m_sortedDeferredDrawCallPointers)
				{
					// Pipeline swap:
					Material* pDeferredMaterial = drawCall->pMaterial;
					VkPipeline newPipeline = pDeferredMaterial->GetPipeline<RenderStage::deferredGeometry>(drawCall->pMesh)->GetVkPipeline();
					bool pipelineLayoutChanged = false;
					if (pipeline != newPipeline)
					{
						// Bind pipeline:
						pipeline = newPipeline;
						vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

						// Pipeline layout swap:
						VkPipelineLayout newPipelineLayout = pDeferredMaterial->GetVkPipelineLayout();
						pipelineLayoutChanged = pipelineLayout != newPipelineLayout;
						if (pipelineLayoutChanged)
						{
							pipelineLayout = newPipelineLayout;

							// Bind static descriptor sets:
							if (!staticDescriptorSetsBound)
							{
								vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 3, m_staticDescriptorSets[m_frameIndex].data(), 0, nullptr);
								staticDescriptorSetsBound = true;
							}
						}
					}

					// Bind per shader descriptor set:
					VkDescriptorSet newShaderDescriptorSet = pDeferredMaterial->GetDescriptorSetBinding()->GetVkDescriptorSet(m_frameIndex);
					if (newShaderDescriptorSet != VK_NULL_HANDLE && (pipelineLayoutChanged || shaderDescriptorSet != newShaderDescriptorSet))
					{
						shaderDescriptorSet = newShaderDescriptorSet;
						vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, SHADER_SET_INDEX, 1, &shaderDescriptorSet, 0, nullptr);
					}

					// Push constant:
					DefaultPushConstant pushConstant(0, drawCall->instanceCount, drawCall->receiveShadows, m_time, m_deltaTime);
					vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(DefaultPushConstant), &pushConstant);

					// Cull mode:
					vkCmdSetCullMode(commandBuffer, CullModeCommonToVulkan(drawCall->cullMode));

					// Bind per draw call descriptor set:
					if (VkDescriptorSet vkDescriptorSet = drawCall->descriptorSetBindingHandle.Get()->GetVkDescriptorSet(m_frameIndex); vkDescriptorSet != VK_NULL_HANDLE)
						vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, CALL_SET_INDEX, 1, &vkDescriptorSet, 0, nullptr);

					// Bind mesh data:
					vkCmdBindVertexBuffers(commandBuffer, 0, drawCall->pMesh->GetVertexBindingCount(), drawCall->pMesh->GetVkBuffers(), drawCall->pMesh->GetOffsets());
					vkCmdBindIndexBuffer(commandBuffer, drawCall->pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, drawCall->pMesh->GetVkIndexType());

					// Dispatch:
					vkCmdDrawIndexed(commandBuffer, drawCall->pMesh->GetIndexCount(), std::max(drawCall->instanceCount, static_cast<uint32_t>(1)), 0, 0, 0);
					DEBUG_LOG_TRACE("Deferred geometry draw call, mesh = {}, material = {}", drawCall->pMesh->GetName(), pDeferredMaterial->GetDebugName());
				}
			}
			vkCmdEndRenderPass(commandBuffer);

			pDeferredGeometryRenderPass->GetAlbedoTexture(m_frameIndex)->GetVmaImage()->SetLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			pDeferredGeometryRenderPass->GetNormalTexture(m_frameIndex)->GetVmaImage()->SetLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			pDeferredGeometryRenderPass->GetMaterialTexture(m_frameIndex)->GetVmaImage()->SetLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			pDeferredGeometryRenderPass->GetDepthTexture(m_frameIndex)->GetVmaImage()->SetLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
		}
		VKA(vkEndCommandBuffer(commandBuffer));
	}
	void Renderer::RecordDeferredLightingCommands()
	{
		PROFILE_FUNCTION();

		// Prepare command recording:
		CommandPool& commandPool = GetCommandPool(m_frameIndex, RenderStage::deferredLighting);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		DeferredLightingRenderPass* pDeferredLightingRenderPass = RenderPassManager::GetDeferredLightingRenderPass();

		// Record deferred lighting commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			// Viewport and scissor:
			VkViewport viewport = {};
			viewport.width = pDeferredLightingRenderPass->GetSceneColorTexture(m_frameIndex)->GetWidth();
			viewport.height = pDeferredLightingRenderPass->GetSceneColorTexture(m_frameIndex)->GetHeight();
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			VkRect2D scissor = {};
			scissor.extent.width = viewport.width;
			scissor.extent.height = viewport.height;
			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

			// Render pass info:
			VkClearValue clearValue = {};
			clearValue.color = { 0.0f, 0.0f, 0.0f, 1.0f };
			VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
			renderPassBeginInfo.renderPass = pDeferredLightingRenderPass->GetVkRenderPass();
			renderPassBeginInfo.framebuffer = pDeferredLightingRenderPass->GetFramebuffer(m_frameIndex);
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent.width = viewport.width;
			renderPassBeginInfo.renderArea.extent.height = viewport.height;
			renderPassBeginInfo.clearValueCount = 1;
			renderPassBeginInfo.pClearValues = &clearValue;

			// Begin render pass:
			Material* pMaterial = DefaultGpuResources::GetDefaultDeferredLightingMaterial();
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			{
				// Bind Pipeline:
				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pMaterial->GetFullscreenPipeline<RenderStage::deferredLighting>()->GetVkPipeline());

				// Bind descriptorSets:
				VkDescriptorSet descriptorSets[4] =
				{
					m_staticDescriptorSets[m_frameIndex][0],
					m_staticDescriptorSets[m_frameIndex][1],
					m_staticDescriptorSets[m_frameIndex][2],
					pMaterial->GetDescriptorSetBinding()->GetVkDescriptorSet(m_frameIndex)
				};
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pMaterial->GetVkPipelineLayout(), 0, 4, descriptorSets, 0, nullptr);

				// Dispatch:
				vkCmdDraw(commandBuffer, 3, 1, 0, 0);
			}
			vkCmdEndRenderPass(commandBuffer);

			pDeferredLightingRenderPass->GetSceneColorTexture(m_frameIndex)->GetVmaImage()->SetLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		}
		VKA(vkEndCommandBuffer(commandBuffer));
	}
	template<RenderStage stage>
	void Renderer::RecordForwardCommands()
	{
		static_assert(stage == RenderStage::forwardOpaque || stage == RenderStage::forwardTransparent);
		PROFILE_FUNCTION();

		RenderPass* pRenderPass;
		RenderTexture2d* pRenderTexture;
		const std::vector<ForwardDrawCall*>* pDrawCallPointers;
		if constexpr (stage == RenderStage::forwardOpaque)
		{
			ForwardOpaqueRenderPass* pForwardOpaqueRenderPass = RenderPassManager::GetForwardOpaqueRenderPass();
			pRenderPass = pForwardOpaqueRenderPass;
			pRenderTexture = pForwardOpaqueRenderPass->GetRenderTexture(m_frameIndex);
			pDrawCallPointers = &m_sortedForwardOpaqueDrawCallPointers;
		}
		else
		{
			ForwardTransparentRenderPass* pForwardTransparentRenderPass = RenderPassManager::GetForwardTransparentRenderPass();
			pRenderPass = pForwardTransparentRenderPass;
			pRenderTexture = pForwardTransparentRenderPass->GetRenderTexture(m_frameIndex);
			pDrawCallPointers = &m_sortedForwardTransparentDrawCallPointers;
		}

		// Prepare command recording:
		CommandPool& commandPool = GetCommandPool(m_frameIndex, stage);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		// Record forward commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			// Viewport and scissor:
			VkViewport viewport = {};
			viewport.width = pRenderTexture->GetWidth();
			viewport.height = pRenderTexture->GetHeight();
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			VkRect2D scissor = {};
			scissor.extent.width = viewport.width;
			scissor.extent.height = viewport.height;
			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

			// Render pass info:
			std::array<VkClearValue, 2> clearValues;
			clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
			clearValues[1].depthStencil = { 1.0f, 0 };
			VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
			renderPassBeginInfo.renderPass = pRenderPass->GetVkRenderPass();
			renderPassBeginInfo.framebuffer = pRenderPass->GetFramebuffer(m_frameIndex);
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent.width = viewport.width;
			renderPassBeginInfo.renderArea.extent.height = viewport.height;
			renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassBeginInfo.pClearValues = clearValues.data();

			// Begin render pass:
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			if (!pDrawCallPointers->empty())
			{
				// Pipeline:
				VkPipeline pipeline = VK_NULL_HANDLE;
				VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
				VkDescriptorSet shaderDescriptorSet = VK_NULL_HANDLE;
				bool staticDescriptorSetsBound = false;

				// Draw calls:
				for (ForwardDrawCall* drawCall : *pDrawCallPointers)
				{
					// Pipeline swap:
					Material* pForwardMaterial = drawCall->pMaterial;
					VkPipeline newPipeline = pForwardMaterial->GetPipeline<stage>(drawCall->pMesh)->GetVkPipeline();
					bool pipelineLayoutChanged = false;
					if (pipeline != newPipeline)
					{
						// Bind Pipeline:
						pipeline = newPipeline;
						vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

						// Pipeline layout swap:
						VkPipelineLayout newPipelineLayout = pForwardMaterial->GetVkPipelineLayout();
						pipelineLayoutChanged = pipelineLayout != newPipelineLayout;
						if (pipelineLayoutChanged)
						{
							pipelineLayout = newPipelineLayout;

							// Bind static descriptor sets:
							if (!staticDescriptorSetsBound)
							{
								vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 3, m_staticDescriptorSets[m_frameIndex].data(), 0, nullptr);
								staticDescriptorSetsBound = true;
							}
						}
					}

					// Bind per shader descriptor set:
					VkDescriptorSet newShaderDescriptorSet = pForwardMaterial->GetDescriptorSetBinding()->GetVkDescriptorSet(m_frameIndex);
					if (newShaderDescriptorSet != VK_NULL_HANDLE && (pipelineLayoutChanged || shaderDescriptorSet != newShaderDescriptorSet))
					{
						shaderDescriptorSet = newShaderDescriptorSet;
						vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, SHADER_SET_INDEX, 1, &shaderDescriptorSet, 0, nullptr);
					}

					// Push constant:
					DefaultPushConstant pushConstant(0, drawCall->instanceCount, drawCall->receiveShadows, m_time, m_deltaTime);
					vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(DefaultPushConstant), &pushConstant);

					// Cull mode:
					vkCmdSetCullMode(commandBuffer, CullModeCommonToVulkan(drawCall->cullMode));

					// Bind per draw call descriptor set:
					if (VkDescriptorSet vkDescriptorSet = drawCall->descriptorSetBindingHandle.Get()->GetVkDescriptorSet(m_frameIndex); vkDescriptorSet != VK_NULL_HANDLE)
						vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, CALL_SET_INDEX, 1, &vkDescriptorSet, 0, nullptr);
					
					// Bind mesh data:
					vkCmdBindVertexBuffers(commandBuffer, 0, drawCall->pMesh->GetVertexBindingCount(), drawCall->pMesh->GetVkBuffers(), drawCall->pMesh->GetOffsets());
					vkCmdBindIndexBuffer(commandBuffer, drawCall->pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, drawCall->pMesh->GetVkIndexType());
					
					// Dispatch:
					vkCmdDrawIndexed(commandBuffer, drawCall->pMesh->GetIndexCount(), std::max(drawCall->instanceCount, (uint32_t)1), 0, 0, 0);
					DEBUG_LOG_TRACE("Forward draw call, mesh = {}, material = {}", drawCall->pMesh->GetName(), pForwardMaterial->GetDebugName());
				}
			}
			vkCmdEndRenderPass(commandBuffer);

			// Make the completed transparent color attachment available to post render compute shaders:
			if constexpr (stage == RenderStage::forwardTransparent)
			{
				VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
				memoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
				memoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
				memoryBarrier.srcAccessMask = AccessMasks::ColorAttachmentOutput::colorAttachmentWrite;
				memoryBarrier.dstAccessMask = AccessMasks::ComputeShader::shaderRead | AccessMasks::ComputeShader::shaderWrite;

				VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
				dependencyInfo.memoryBarrierCount = 1;
				dependencyInfo.pMemoryBarriers = &memoryBarrier;

				vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
				DEBUG_LOG_TRACE("Memory Barrier: forward transparent color attachment to post compute");
			}
		}
		VKA(vkEndCommandBuffer(commandBuffer));

		if constexpr (stage == RenderStage::forwardTransparent)
			pRenderTexture->GetVmaImage()->SetLayout(VK_IMAGE_LAYOUT_GENERAL);
		else
			pRenderTexture->GetVmaImage()->SetLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	}
	void Renderer::RecordPostRenderComputeCommands()
	{
		PROFILE_FUNCTION();

		// Prepare command recording:
		CommandPool& commandPool = GetCommandPool(m_frameIndex, RenderStage::postRenderCompute);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		// Record post render compute commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			// Transition final scene color to general layout for compute shader access:
			m_pSceneColorTexturePair->PrepareForPostProcessing(commandBuffer, m_frameIndex);

			PostRender* pPostRenderCompute = m_pCompute->GetPostRenderCompute();
			std::vector<ComputeCall>& computeCalls = pPostRenderCompute->GetComputeCalls();

			// Pipeline:
			VkPipeline pipeline = VK_NULL_HANDLE;
			VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
			bool staticDescriptorSetsBound = false;

			for (size_t computeCallIndex = 0; computeCallIndex < computeCalls.size(); computeCallIndex++)
			{
				ComputeCall* computeCall = &computeCalls[computeCallIndex];
				ComputeShader* pComputeShader = computeCall->GetComputeShader();

				// Pipeline change:
				VkPipeline newPipeline = pComputeShader->GetPipeline()->GetVkPipeline();
				if (pipeline != newPipeline)
				{
					// Bind Pipeline:
					pipeline = newPipeline;
					pipelineLayout = pComputeShader->GetVkPipelineLayout();
					vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);

					// Bind static descriptor sets:
					if (!staticDescriptorSetsBound)
					{
						vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 3, m_staticDescriptorSets[m_frameIndex].data(), 0, nullptr);
						staticDescriptorSetsBound = true;
					}

					// Bind per shader descriptor set:
					if (VkDescriptorSet vkDescriptorSet = pComputeShader->GetDescriptorSetBinding()->GetVkDescriptorSet(m_frameIndex); vkDescriptorSet != VK_NULL_HANDLE)
						vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, SHADER_SET_INDEX, 1, &vkDescriptorSet, 0, nullptr);
				}

				// Bind per compute call descriptor set:
				if (VkDescriptorSet vkDescriptorSet = computeCall->callDescriptorSetBindingHandle.Get()->GetVkDescriptorSet(m_frameIndex); vkDescriptorSet != VK_NULL_HANDLE)
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, CALL_SET_INDEX, 1, &vkDescriptorSet, 0, nullptr);

				// Push constant:
				ComputePushConstant pushConstant(computeCall->threadCount, m_time, m_deltaTime);
				vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstant), &pushConstant);

				// Group counts:
				Uint3 blockSize = pComputeShader->GetBlockSize();
				uint32_t groupCountX = (computeCall->threadCount.x + blockSize.x - 1) / blockSize.x;
				uint32_t groupCountY = (computeCall->threadCount.y + blockSize.y - 1) / blockSize.y;
				uint32_t groupCountZ = (computeCall->threadCount.z + blockSize.z - 1) / blockSize.z;

				// Dispatch:
				vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
				DEBUG_LOG_TRACE("Post Render Compute Shader {}, call = {}", pComputeShader->GetDebugName(), computeCallIndex);

				// Post-render compute shaders execute in recorded order and may access the same resources:
				VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
				memoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
				memoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
				memoryBarrier.srcAccessMask = AccessMasks::ComputeShader::shaderWrite;
				memoryBarrier.dstAccessMask = AccessMasks::ComputeShader::shaderRead | AccessMasks::ComputeShader::shaderWrite;

				VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
				dependencyInfo.memoryBarrierCount = 1;
				dependencyInfo.pMemoryBarriers = &memoryBarrier;

				vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
				DEBUG_LOG_TRACE("Post Render Compute Barrier, call = {}", computeCallIndex);
			}
		}
		VKA(vkEndCommandBuffer(commandBuffer));
	}
	void Renderer::RecordPresentCommands()
	{
		PROFILE_FUNCTION();

		// Prepare command recording:
		CommandPool& commandPool = GetCommandPool(m_frameIndex, RenderStage::present);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		// Record present commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			// Publish the final scene color for editor sampling:
			m_pSceneColorTexturePair->PrepareCurrentForSampling(commandBuffer, m_frameIndex);

			DescriptorSetBinding* pPresentShaderDescriptorSetBinding = DefaultGpuResources::GetDefaultPresentMaterial()->GetDescriptorSetBinding();
			pPresentShaderDescriptorSetBinding->SetTexture("renderTexture", m_pSceneColorTexturePair->GetCurrentTexture(m_frameIndex));
			pPresentShaderDescriptorSetBinding->SetTexture("gizmoTexture", RenderPassManager::GetGizmoRenderPass()->GetRenderTexture(m_frameIndex));
			pPresentShaderDescriptorSetBinding->UpdateShaderData(m_frameIndex);

			// Viewport and scissor:
			Uint2 swapchainExtent = Context::GetSwapchain()->GetExtent();
			VkViewport viewport = {};
			viewport.width = (float)swapchainExtent.x;
			viewport.height = (float)swapchainExtent.y;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			VkRect2D scissor = {};
			scissor.extent.width = viewport.width;
			scissor.extent.height = viewport.height;
			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

			// Render pass info:
			PresentRenderPass* presentRenderPass = RenderPassManager::GetPresentRenderPass();
			VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
			renderPassBeginInfo.renderPass = presentRenderPass->GetVkRenderPass();
			renderPassBeginInfo.framebuffer = presentRenderPass->GetFramebuffer(m_imageIndex);
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent = VkExtent2D{swapchainExtent.x, swapchainExtent.y};

			// Begin render pass:
			Material* pMaterial = DefaultGpuResources::GetDefaultPresentMaterial();
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			{
				// Bind Pipeline:
				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pMaterial->GetFullscreenPipeline<RenderStage::present>()->GetVkPipeline());

				// Bind descriptorSets:
				VkDescriptorSet descriptorSets[4] =
				{
					m_staticDescriptorSets[m_frameIndex][0],
					m_staticDescriptorSets[m_frameIndex][1],
					m_staticDescriptorSets[m_frameIndex][2],
					pMaterial->GetDescriptorSetBinding()->GetVkDescriptorSet(m_frameIndex)
				};
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pMaterial->GetVkPipelineLayout(), 0, 4, descriptorSets, 0, nullptr);
				
				// Dispatch:
				vkCmdDraw(commandBuffer, 3, 1, 0, 0);
				if (m_pIGui)
					m_pIGui->Render(commandBuffer);
				DEBUG_LOG_INFO("Render renderTexture with fullscreen triangle, material = {}", pMaterial->GetDebugName());
			}
			vkCmdEndRenderPass(commandBuffer);

		}
		VKA(vkEndCommandBuffer(commandBuffer));
	}
	void Renderer::RecordImGuiPresentCommands()
	{
		PROFILE_FUNCTION();

		// Prepare command recording:
		CommandPool& commandPool = GetCommandPool(m_frameIndex, RenderStage::present);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		// Record present commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			// Publish the final scene color for editor sampling:
			m_pSceneColorTexturePair->PrepareCurrentForSampling(commandBuffer, m_frameIndex);

			// Render pass info:
			Uint2 swapchainExtent = Context::GetSwapchain()->GetExtent();
			PresentRenderPass* presentRenderPass = RenderPassManager::GetPresentRenderPass();
			VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
			renderPassBeginInfo.renderPass = presentRenderPass->GetVkRenderPass();
			renderPassBeginInfo.framebuffer = presentRenderPass->GetFramebuffer(m_imageIndex);
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent = VkExtent2D{swapchainExtent.x, swapchainExtent.y};

			// Begin render pass:
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			{
				if (m_pIGui)
					m_pIGui->Render(commandBuffer);
			}
			vkCmdEndRenderPass(commandBuffer);

		}
		VKA(vkEndCommandBuffer(commandBuffer));
	}



	// Submit commands:
	void Renderer::SubmitResourceUpdateCommands()
	{
		CommandPool& commandPool = GetCommandPool(m_frameIndex, RenderStage::resourceUpdate);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();

		// Wait semaphore info:
		VkSemaphoreSubmitInfo waitSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		waitSemaphoreInfo.semaphore = m_acquireSemaphores[m_frameIndex];
		waitSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;

		// Command buffer info:
		VkCommandBufferSubmitInfo commandBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
		commandBufferInfo.commandBuffer = commandBuffer;

		// Signal semaphore info:
		std::array<VkSemaphoreSubmitInfo, 2> signalSemaphoreInfos{};
		signalSemaphoreInfos[0].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		signalSemaphoreInfos[0].semaphore = m_resourceUpdateToPreRenderComputeSemaphores[m_frameIndex];
		signalSemaphoreInfos[0].stageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
		signalSemaphoreInfos[1].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		signalSemaphoreInfos[1].semaphore = m_resourceUpdateToGizmoSemaphores[m_frameIndex];
		signalSemaphoreInfos[1].stageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;

		// Submit info:
		VkSubmitInfo2 submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
		submitInfo.waitSemaphoreInfoCount = 1;
		submitInfo.pWaitSemaphoreInfos = &waitSemaphoreInfo;
		submitInfo.commandBufferInfoCount = 1;
		submitInfo.pCommandBufferInfos = &commandBufferInfo;
		submitInfo.signalSemaphoreInfoCount = static_cast<uint32_t>(signalSemaphoreInfos.size());
		submitInfo.pSignalSemaphoreInfos = signalSemaphoreInfos.data();

		// Submit:
		VKA(vkQueueSubmit2(Context::GetLogicalDevice()->GetGraphicsQueue().queue, 1, &submitInfo, VK_NULL_HANDLE));
	}
	void Renderer::SubmitGizmoCommands()
	{
		CommandPool& commandPool = GetCommandPool(m_frameIndex, RenderStage::gizmo);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();

		// Wait semaphore info:
		VkSemaphoreSubmitInfo waitSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		waitSemaphoreInfo.semaphore = m_resourceUpdateToGizmoSemaphores[m_frameIndex];
		waitSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

		// Command buffer info:
		VkCommandBufferSubmitInfo commandBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
		commandBufferInfo.commandBuffer = commandBuffer;

		// Signal semaphore info:
		VkSemaphoreSubmitInfo signalSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		signalSemaphoreInfo.semaphore = m_gizmoToPresentSemaphores[m_frameIndex];
		signalSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

		// Submit info:
		VkSubmitInfo2 submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
		submitInfo.waitSemaphoreInfoCount = 1;
		submitInfo.pWaitSemaphoreInfos = &waitSemaphoreInfo;
		submitInfo.commandBufferInfoCount = 1;
		submitInfo.pCommandBufferInfos = &commandBufferInfo;
		submitInfo.signalSemaphoreInfoCount = 1;
		submitInfo.pSignalSemaphoreInfos = &signalSemaphoreInfo;

		// Submit:
		VKA(vkQueueSubmit2(Context::GetLogicalDevice()->GetGraphicsQueue().queue, 1, &submitInfo, VK_NULL_HANDLE));
	}
    void Renderer::SubmitPreRenderComputeCommands()
	{
		CommandPool& commandPool = GetCommandPool(m_frameIndex, RenderStage::preRenderCompute);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();

		// Wait semaphore info:
		VkSemaphoreSubmitInfo waitSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		waitSemaphoreInfo.semaphore = m_resourceUpdateToPreRenderComputeSemaphores[m_frameIndex];
		waitSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;

		// Command buffer info:
		VkCommandBufferSubmitInfo commandBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
		commandBufferInfo.commandBuffer = commandBuffer;

		// Signal semaphore info:
		std::array<VkSemaphoreSubmitInfo, 3> signalSemaphoreInfos{};
		signalSemaphoreInfos[0].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		signalSemaphoreInfos[0].semaphore = m_preRenderComputeToShadowSemaphores[m_frameIndex];
		signalSemaphoreInfos[0].stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
		signalSemaphoreInfos[1].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		signalSemaphoreInfos[1].semaphore = m_preRenderComputeToDeferredGeometrySemaphores[m_frameIndex];
		signalSemaphoreInfos[1].stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
		signalSemaphoreInfos[2].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		signalSemaphoreInfos[2].semaphore = m_preRenderComputeToOutlineSemaphores[m_frameIndex];
		signalSemaphoreInfos[2].stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;

		// Submit info:
		VkSubmitInfo2 submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
		submitInfo.waitSemaphoreInfoCount = 1;
		submitInfo.pWaitSemaphoreInfos = &waitSemaphoreInfo;
		submitInfo.commandBufferInfoCount = 1;
		submitInfo.pCommandBufferInfos = &commandBufferInfo;
		submitInfo.signalSemaphoreInfoCount = static_cast<uint32_t>(signalSemaphoreInfos.size());
		submitInfo.pSignalSemaphoreInfos = signalSemaphoreInfos.data();

		// Submit:
		VKA(vkQueueSubmit2(Context::GetLogicalDevice()->GetGraphicsQueue().queue, 1, &submitInfo, VK_NULL_HANDLE));
	}
	void Renderer::SubmitOutlineCommands()
	{
		CommandPool& commandPool = GetCommandPool(m_frameIndex, RenderStage::outline);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();

		// Wait semaphore info:
		VkSemaphoreSubmitInfo waitSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		waitSemaphoreInfo.semaphore = m_preRenderComputeToOutlineSemaphores[m_frameIndex];
		waitSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

		// Command buffer info:
		VkCommandBufferSubmitInfo commandBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
		commandBufferInfo.commandBuffer = commandBuffer;

		// Signal semaphore info:
		VkSemaphoreSubmitInfo signalSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		signalSemaphoreInfo.semaphore = m_outlineToRenderComputeSemaphores[m_frameIndex];
		signalSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

		// Submit info:
		VkSubmitInfo2 submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
		submitInfo.waitSemaphoreInfoCount = 1;
		submitInfo.pWaitSemaphoreInfos = &waitSemaphoreInfo;
		submitInfo.commandBufferInfoCount = 1;
		submitInfo.pCommandBufferInfos = &commandBufferInfo;
		submitInfo.signalSemaphoreInfoCount = 1;
		submitInfo.pSignalSemaphoreInfos = &signalSemaphoreInfo;

		// Submit:
		VKA(vkQueueSubmit2(Context::GetLogicalDevice()->GetGraphicsQueue().queue, 1, &submitInfo, VK_NULL_HANDLE));
	}
	void Renderer::SubmitRenderComputeCommands()
	{
		CommandPool& commandPool = GetCommandPool(m_frameIndex, RenderStage::renderCompute);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();

		// Wait semaphore info:
		VkSemaphoreSubmitInfo waitSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		waitSemaphoreInfo.semaphore = m_outlineToRenderComputeSemaphores[m_frameIndex];
		waitSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;

		// Command buffer info:
		VkCommandBufferSubmitInfo commandBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
		commandBufferInfo.commandBuffer = commandBuffer;

		// Signal semaphore info:
		VkSemaphoreSubmitInfo signalSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		signalSemaphoreInfo.semaphore = m_renderComputeToPostRenderComputeSemaphores[m_frameIndex];
		signalSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;

		// Submit info:
		VkSubmitInfo2 submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
		submitInfo.waitSemaphoreInfoCount = 1;
		submitInfo.pWaitSemaphoreInfos = &waitSemaphoreInfo;
		submitInfo.commandBufferInfoCount = 1;
		submitInfo.pCommandBufferInfos = &commandBufferInfo;
		submitInfo.signalSemaphoreInfoCount = 1;
		submitInfo.pSignalSemaphoreInfos = &signalSemaphoreInfo;

		// Submit:
		VKA(vkQueueSubmit2(Context::GetLogicalDevice()->GetComputeQueue().queue, 1, &submitInfo, VK_NULL_HANDLE));
	}
    void Renderer::SubmitShadowCommands()
	{
		CommandPool& commandPool = GetCommandPool(m_frameIndex, RenderStage::shadow);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();

		// Wait semaphore info:
		VkSemaphoreSubmitInfo waitSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		waitSemaphoreInfo.semaphore = m_preRenderComputeToShadowSemaphores[m_frameIndex];
		// Shadow draws can immediately consume mesh buffers uploaded in the resourceUpdate submission.
		// The wait therefore has to block the front of the graphics pipeline, including index/vertex fetch,
		// before the shadow pass starts reading those buffers.
		waitSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;

		// Command buffer info:
		VkCommandBufferSubmitInfo commandBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
		commandBufferInfo.commandBuffer = commandBuffer;

		// Signal semaphore info:
		VkSemaphoreSubmitInfo signalSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		signalSemaphoreInfo.semaphore = m_shadowToDeferredLightingSemaphores[m_frameIndex];
		signalSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;

		// Submit info:
		VkSubmitInfo2 submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
		submitInfo.waitSemaphoreInfoCount = 1;
		submitInfo.pWaitSemaphoreInfos = &waitSemaphoreInfo;
		submitInfo.commandBufferInfoCount = 1;
		submitInfo.pCommandBufferInfos = &commandBufferInfo;
		submitInfo.signalSemaphoreInfoCount = 1;
		submitInfo.pSignalSemaphoreInfos = &signalSemaphoreInfo;

		// Submit:
		VKA(vkQueueSubmit2(Context::GetLogicalDevice()->GetGraphicsQueue().queue, 1, &submitInfo, VK_NULL_HANDLE));
	}
	void Renderer::SubmitDeferredGeometryCommands()
	{
		CommandPool& commandPool = GetCommandPool(m_frameIndex, RenderStage::deferredGeometry);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();

		// Wait semaphore info:
		VkSemaphoreSubmitInfo waitSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		waitSemaphoreInfo.semaphore = m_preRenderComputeToDeferredGeometrySemaphores[m_frameIndex];
		waitSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

		// Command buffer info:
		VkCommandBufferSubmitInfo commandBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
		commandBufferInfo.commandBuffer = commandBuffer;

		// Signal semaphore info:
		VkSemaphoreSubmitInfo signalSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		signalSemaphoreInfo.semaphore = m_deferredGeometryToDeferredLightingSemaphores[m_frameIndex];
		signalSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;

		// Submit info:
		VkSubmitInfo2 submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
		submitInfo.waitSemaphoreInfoCount = 1;
		submitInfo.pWaitSemaphoreInfos = &waitSemaphoreInfo;
		submitInfo.commandBufferInfoCount = 1;
		submitInfo.pCommandBufferInfos = &commandBufferInfo;
		submitInfo.signalSemaphoreInfoCount = 1;
		submitInfo.pSignalSemaphoreInfos = &signalSemaphoreInfo;

		// Submit:
		VKA(vkQueueSubmit2(Context::GetLogicalDevice()->GetGraphicsQueue().queue, 1, &submitInfo, VK_NULL_HANDLE));
	}
	void Renderer::SubmitDeferredLightingCommands()
	{
		CommandPool& commandPool = GetCommandPool(m_frameIndex, RenderStage::deferredLighting);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();

		// Wait semaphore info:
		std::array<VkSemaphoreSubmitInfo, 2> waitSemaphoreInfos{};
		waitSemaphoreInfos[0].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		waitSemaphoreInfos[0].semaphore = m_shadowToDeferredLightingSemaphores[m_frameIndex];
		waitSemaphoreInfos[0].stageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
		waitSemaphoreInfos[1].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		waitSemaphoreInfos[1].semaphore = m_deferredGeometryToDeferredLightingSemaphores[m_frameIndex];
		waitSemaphoreInfos[1].stageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

		// Command buffer info:
		VkCommandBufferSubmitInfo commandBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
		commandBufferInfo.commandBuffer = commandBuffer;

		// Signal semaphore info:
		VkSemaphoreSubmitInfo signalSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		signalSemaphoreInfo.semaphore = m_deferredLightingToForwardOpaqueSemaphores[m_frameIndex];
		signalSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

		// Submit info:
		VkSubmitInfo2 submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
		submitInfo.waitSemaphoreInfoCount = static_cast<uint32_t>(waitSemaphoreInfos.size());
		submitInfo.pWaitSemaphoreInfos = waitSemaphoreInfos.data();
		submitInfo.commandBufferInfoCount = 1;
		submitInfo.pCommandBufferInfos = &commandBufferInfo;
		submitInfo.signalSemaphoreInfoCount = 1;
		submitInfo.pSignalSemaphoreInfos = &signalSemaphoreInfo;

		// Submit:
		VKA(vkQueueSubmit2(Context::GetLogicalDevice()->GetGraphicsQueue().queue, 1, &submitInfo, VK_NULL_HANDLE));
	}
	void Renderer::SubmitForwardOpaqueCommands()
	{
		CommandPool& commandPool = GetCommandPool(m_frameIndex, RenderStage::forwardOpaque);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		
		// Wait semaphore info:
		VkSemaphoreSubmitInfo waitSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		waitSemaphoreInfo.semaphore = m_deferredLightingToForwardOpaqueSemaphores[m_frameIndex];
		// Deferred lighting joins shadow and deferred geometry, so this wait makes their resources and the
		// completed scene attachments available to every graphics stage used by forward rendering.
		waitSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

		// Command buffer info:
		VkCommandBufferSubmitInfo commandBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
		commandBufferInfo.commandBuffer = commandBuffer;

		// Signal semaphore info:
		VkSemaphoreSubmitInfo signalSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		signalSemaphoreInfo.semaphore = m_forwardOpaqueToForwardTransparentSemaphores[m_frameIndex];
		signalSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;

		// Submit info:
		VkSubmitInfo2 submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
		submitInfo.waitSemaphoreInfoCount = 1;
		submitInfo.pWaitSemaphoreInfos = &waitSemaphoreInfo;
		submitInfo.commandBufferInfoCount = 1;
		submitInfo.pCommandBufferInfos = &commandBufferInfo;
		submitInfo.signalSemaphoreInfoCount = 1;
		submitInfo.pSignalSemaphoreInfos = &signalSemaphoreInfo;

		// Submit:
		VKA(vkQueueSubmit2(Context::GetLogicalDevice()->GetGraphicsQueue().queue, 1, &submitInfo, VK_NULL_HANDLE));
	}
	void Renderer::SubmitForwardTransparentCommands()
	{
		CommandPool& commandPool = GetCommandPool(m_frameIndex, RenderStage::forwardTransparent);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();

		// Wait semaphore info:
		VkSemaphoreSubmitInfo waitSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		waitSemaphoreInfo.semaphore = m_forwardOpaqueToForwardTransparentSemaphores[m_frameIndex];
		waitSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

		// Command buffer info:
		VkCommandBufferSubmitInfo commandBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
		commandBufferInfo.commandBuffer = commandBuffer;

		// Signal semaphore info:
		VkSemaphoreSubmitInfo signalSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		signalSemaphoreInfo.semaphore = m_forwardTransparentToPostRenderComputeSemaphores[m_frameIndex];
		signalSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

		// Submit info:
		VkSubmitInfo2 submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
		submitInfo.waitSemaphoreInfoCount = 1;
		submitInfo.pWaitSemaphoreInfos = &waitSemaphoreInfo;
		submitInfo.commandBufferInfoCount = 1;
		submitInfo.pCommandBufferInfos = &commandBufferInfo;
		submitInfo.signalSemaphoreInfoCount = 1;
		submitInfo.pSignalSemaphoreInfos = &signalSemaphoreInfo;

		// Submit:
		VKA(vkQueueSubmit2(Context::GetLogicalDevice()->GetGraphicsQueue().queue, 1, &submitInfo, VK_NULL_HANDLE));
	}
	void Renderer::SubmitPostRenderComputeCommands()
	{
		CommandPool& commandPool = GetCommandPool(m_frameIndex, RenderStage::postRenderCompute);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();

		// Wait semaphore info:
		std::array<VkSemaphoreSubmitInfo, 2> waitSemaphoreInfos{};
		waitSemaphoreInfos[0].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		waitSemaphoreInfos[0].semaphore = m_forwardTransparentToPostRenderComputeSemaphores[m_frameIndex];
		waitSemaphoreInfos[0].stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
		waitSemaphoreInfos[1].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		waitSemaphoreInfos[1].semaphore = m_renderComputeToPostRenderComputeSemaphores[m_frameIndex];
		waitSemaphoreInfos[1].stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;

		// Command buffer info:
		VkCommandBufferSubmitInfo commandBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
		commandBufferInfo.commandBuffer = commandBuffer;

		// Signal semaphore info:
		VkSemaphoreSubmitInfo signalSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		signalSemaphoreInfo.semaphore = m_postRenderComputeToPresentSemaphores[m_frameIndex];
		signalSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;

		// Submit info:
		VkSubmitInfo2 submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
		submitInfo.waitSemaphoreInfoCount = static_cast<uint32_t>(waitSemaphoreInfos.size());
		submitInfo.pWaitSemaphoreInfos = waitSemaphoreInfos.data();
		submitInfo.commandBufferInfoCount = 1;
		submitInfo.pCommandBufferInfos = &commandBufferInfo;
		submitInfo.signalSemaphoreInfoCount = 1;
		submitInfo.pSignalSemaphoreInfos = &signalSemaphoreInfo;

		// Submit:
		VKA(vkQueueSubmit2(Context::GetLogicalDevice()->GetGraphicsQueue().queue, 1, &submitInfo, VK_NULL_HANDLE));
	}
	void Renderer::SubmitPresentCommands()
	{
		CommandPool& commandPool = GetCommandPool(m_frameIndex, RenderStage::present);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();

		// Wait semaphore info:
		std::array<VkSemaphoreSubmitInfo, 2> waitSemaphoreInfos{};
		waitSemaphoreInfos[0].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		waitSemaphoreInfos[0].semaphore = m_postRenderComputeToPresentSemaphores[m_frameIndex];
		waitSemaphoreInfos[0].stageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
		waitSemaphoreInfos[1].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		waitSemaphoreInfos[1].semaphore = m_gizmoToPresentSemaphores[m_frameIndex];
		waitSemaphoreInfos[1].stageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;

		// Command buffer info:
		VkCommandBufferSubmitInfo commandBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
		commandBufferInfo.commandBuffer = commandBuffer;

		// Signal semaphore info:
		VkSemaphoreSubmitInfo signalSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		signalSemaphoreInfo.semaphore = m_releaseSemaphores[m_imageIndex];
		signalSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;

		// Submit info:
		VkSubmitInfo2 submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
		submitInfo.waitSemaphoreInfoCount = static_cast<uint32_t>(waitSemaphoreInfos.size());
		submitInfo.pWaitSemaphoreInfos = waitSemaphoreInfos.data();
		submitInfo.commandBufferInfoCount = 1;
		submitInfo.pCommandBufferInfos = &commandBufferInfo;
		submitInfo.signalSemaphoreInfoCount = 1;
		submitInfo.pSignalSemaphoreInfos = &signalSemaphoreInfo;

		// Submit:
		VKA(vkQueueSubmit2(Context::GetLogicalDevice()->GetGraphicsQueue().queue, 1, &submitInfo, m_frameFences[m_frameIndex]));
	}
	bool Renderer::PresentImage()
	{
		PROFILE_FUNCTION();
		VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_releaseSemaphores[m_imageIndex];
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &Context::GetVkSwapchainKHR();
		presentInfo.pImageIndices = &m_imageIndex;

		VkResult result = vkQueuePresentKHR(Context::GetLogicalDevice()->GetPresentQueue().queue, &presentInfo);
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



	// Sync objects management:
	void Renderer::CreateFences()
	{
		VkFenceCreateInfo createInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
		createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;	// Fence is created in the signaled state to prevent the first wait from blocking.

		m_frameFences.resize(Context::GetFramesInFlight());
		for (uint32_t i = 0; i < Context::GetFramesInFlight(); i++)
		{
			VKA(vkCreateFence(Context::GetVkDevice(), &createInfo, nullptr, &m_frameFences[i]));
			NAME_VK_OBJECT(m_frameFences[i], "Fence_Frame" + std::to_string(i));
		}
	}
	void Renderer::CreateSemaphores()
	{
		VkSemaphoreCreateInfo createInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

		// One per frame in flight:
		m_acquireSemaphores.resize(Context::GetFramesInFlight());
		m_resourceUpdateToPreRenderComputeSemaphores.resize(Context::GetFramesInFlight());
		m_resourceUpdateToGizmoSemaphores.resize(Context::GetFramesInFlight());
		m_preRenderComputeToShadowSemaphores.resize(Context::GetFramesInFlight());
		m_preRenderComputeToDeferredGeometrySemaphores.resize(Context::GetFramesInFlight());
		m_preRenderComputeToOutlineSemaphores.resize(Context::GetFramesInFlight());
		m_shadowToDeferredLightingSemaphores.resize(Context::GetFramesInFlight());
		m_deferredGeometryToDeferredLightingSemaphores.resize(Context::GetFramesInFlight());
		m_deferredLightingToForwardOpaqueSemaphores.resize(Context::GetFramesInFlight());
		m_forwardOpaqueToForwardTransparentSemaphores.resize(Context::GetFramesInFlight());
		m_forwardTransparentToPostRenderComputeSemaphores.resize(Context::GetFramesInFlight());
		m_outlineToRenderComputeSemaphores.resize(Context::GetFramesInFlight());
		m_renderComputeToPostRenderComputeSemaphores.resize(Context::GetFramesInFlight());
		m_gizmoToPresentSemaphores.resize(Context::GetFramesInFlight());
		m_postRenderComputeToPresentSemaphores.resize(Context::GetFramesInFlight());
		m_releaseSemaphores.resize(Context::GetSwapchain()->GetImageCount());
		for (uint32_t i = 0; i < Context::GetFramesInFlight(); i++)
		{
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_acquireSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_resourceUpdateToPreRenderComputeSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_resourceUpdateToGizmoSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_preRenderComputeToShadowSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_preRenderComputeToDeferredGeometrySemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_preRenderComputeToOutlineSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_shadowToDeferredLightingSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_deferredGeometryToDeferredLightingSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_deferredLightingToForwardOpaqueSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_forwardOpaqueToForwardTransparentSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_forwardTransparentToPostRenderComputeSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_outlineToRenderComputeSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_renderComputeToPostRenderComputeSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_gizmoToPresentSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_postRenderComputeToPresentSemaphores[i]));
			NAME_VK_OBJECT(m_acquireSemaphores[i], "Semaphore_Acquire_Frame" + std::to_string(i));
			NAME_VK_OBJECT(m_resourceUpdateToPreRenderComputeSemaphores[i], "Semaphore_ResourceUpdateToPreRenderCompute_Frame" + std::to_string(i));
			NAME_VK_OBJECT(m_resourceUpdateToGizmoSemaphores[i], "Semaphore_ResourceUpdateToGizmo_Frame" + std::to_string(i));
			NAME_VK_OBJECT(m_preRenderComputeToShadowSemaphores[i], "Semaphore_PreRenderComputeToShadow_Frame" + std::to_string(i));
			NAME_VK_OBJECT(m_preRenderComputeToDeferredGeometrySemaphores[i], "Semaphore_PreRenderComputeToDeferredGeometry_Frame" + std::to_string(i));
			NAME_VK_OBJECT(m_preRenderComputeToOutlineSemaphores[i], "Semaphore_PreRenderComputeToOutline_Frame" + std::to_string(i));
			NAME_VK_OBJECT(m_shadowToDeferredLightingSemaphores[i], "Semaphore_ShadowToDeferredLighting_Frame" + std::to_string(i));
			NAME_VK_OBJECT(m_deferredGeometryToDeferredLightingSemaphores[i], "Semaphore_DeferredGeometryToDeferredLighting_Frame" + std::to_string(i));
			NAME_VK_OBJECT(m_deferredLightingToForwardOpaqueSemaphores[i], "Semaphore_DeferredLightingToForwardOpaque_Frame" + std::to_string(i));
			NAME_VK_OBJECT(m_forwardOpaqueToForwardTransparentSemaphores[i], "Semaphore_ForwardOpaqueToForwardTransparent_Frame" + std::to_string(i));
			NAME_VK_OBJECT(m_forwardTransparentToPostRenderComputeSemaphores[i], "Semaphore_ForwardTransparentToPostRenderCompute_Frame" + std::to_string(i));
			NAME_VK_OBJECT(m_outlineToRenderComputeSemaphores[i], "Semaphore_OutlineToRenderCompute_Frame" + std::to_string(i));
			NAME_VK_OBJECT(m_renderComputeToPostRenderComputeSemaphores[i], "Semaphore_RenderComputeToPostRenderCompute_Frame" + std::to_string(i));
			NAME_VK_OBJECT(m_gizmoToPresentSemaphores[i], "Semaphore_GizmoToPresent_Frame" + std::to_string(i));
			NAME_VK_OBJECT(m_postRenderComputeToPresentSemaphores[i], "Semaphore_PostRenderToPresent_Frame" + std::to_string(i));
		}
		for (uint32_t i = 0; i < m_releaseSemaphores.size(); i++)
		{
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_releaseSemaphores[i]));
			NAME_VK_OBJECT(m_releaseSemaphores[i], "Semaphore_Release_SwapchainImage" + std::to_string(i));
		}
	}
	void Renderer::DestroyFences()
	{
		for (uint32_t i = 0; i < Context::GetFramesInFlight(); i++)
			vkDestroyFence(Context::GetVkDevice(), m_frameFences[i], nullptr);
		m_frameFences.clear();
	}
	void Renderer::DestroySemaphores()
	{
		for (uint32_t i = 0; i < Context::GetFramesInFlight(); i++)
		{
			vkDestroySemaphore(Context::GetVkDevice(), m_acquireSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_resourceUpdateToPreRenderComputeSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_resourceUpdateToGizmoSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_preRenderComputeToShadowSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_preRenderComputeToDeferredGeometrySemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_preRenderComputeToOutlineSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_shadowToDeferredLightingSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_deferredGeometryToDeferredLightingSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_deferredLightingToForwardOpaqueSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_forwardOpaqueToForwardTransparentSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_forwardTransparentToPostRenderComputeSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_outlineToRenderComputeSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_renderComputeToPostRenderComputeSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_gizmoToPresentSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_postRenderComputeToPresentSemaphores[i], nullptr);
		}
		for (uint32_t i = 0; i < m_releaseSemaphores.size(); i++)
			vkDestroySemaphore(Context::GetVkDevice(), m_releaseSemaphores[i], nullptr);
		m_acquireSemaphores.clear();
		m_resourceUpdateToPreRenderComputeSemaphores.clear();
		m_resourceUpdateToGizmoSemaphores.clear();
		m_preRenderComputeToShadowSemaphores.clear();
		m_preRenderComputeToDeferredGeometrySemaphores.clear();
		m_preRenderComputeToOutlineSemaphores.clear();
		m_shadowToDeferredLightingSemaphores.clear();
		m_deferredGeometryToDeferredLightingSemaphores.clear();
		m_deferredLightingToForwardOpaqueSemaphores.clear();
		m_forwardOpaqueToForwardTransparentSemaphores.clear();
		m_forwardTransparentToPostRenderComputeSemaphores.clear();
		m_outlineToRenderComputeSemaphores.clear();
		m_renderComputeToPostRenderComputeSemaphores.clear();
		m_gizmoToPresentSemaphores.clear();
		m_postRenderComputeToPresentSemaphores.clear();
		m_releaseSemaphores.clear();
	}
	
	

	// Internal getters:
	CommandPool& Renderer::GetCommandPool(int frameIndex, RenderStage renderStage)
	{
		return GetCommandPool(frameIndex, (int)renderStage);
	}
	CommandPool& Renderer::GetCommandPool(int frameIndex, int renderStage)
	{
		// Fast index: frameIndex
		// Slow index: renderStage
		// preRenderComputeCommandBufferFrame0
		// preRenderComputeCommandBufferFrame1
		// shadowCommandBufferFrame0
		// shadowCommandBufferFrame1
		// ...
		assert(renderStage < (int)RenderStage::stageCount);
		assert(frameIndex < Context::GetFramesInFlight());
		return m_commandPools[frameIndex + (int)renderStage * Context::GetFramesInFlight()];
	}
}