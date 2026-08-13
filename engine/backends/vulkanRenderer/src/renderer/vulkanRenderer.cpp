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
#include "vulkanCommandPool.h"
#include "vulkanCompute.h"
#include "vulkanComputeCall.h"
#include "vulkanComputePushConstant.h"
#include "vulkanComputeShader.h"
#include "vulkanContext.h"
#include "vulkanConvertMaterialRenderState.h"
#include "vulkanConvertTextureFormat.h"
#include "vulkanDefaultGpuResources.h"
#include "vulkanDepthTexture2dArray.h"
#include "vulkanDefaultPushConstant.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanDrawCall.h"
#include "vulkanFrameDescriptorSetLayout.h"
#include "vulkanForwardRenderPass.h"
#include "vulkanGarbageCollector.h"
#include "vulkanGizmoRenderPass.h"
#include "vulkanGlobalDescriptorSetLayout.h"
#include "vulkanIndexBuffer.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include "vulkanMaterial.h"
#include "vulkanMesh.h"
#include "vulkanOutlineRenderPass.h"
#include "vulkanPipeline.h"
#include "vulkanPoolManager.h"
#include "vulkanPostRenderCompute.h"
#include "vulkanPreRenderCompute.h"
#include "vulkanPresentRenderPass.h"
#include "vulkanRenderPassManager.h"
#include "vulkanRenderTexture2d.h"
#include "vulkanSampler.h"
#include "vulkanSampleTexture2d.h"
#include "vulkanSampleTexture3d.h"
#include "vulkanSampleTextureCube.h"
#include "vulkanSceneDescriptorSetLayout.h"
#include "vulkanShadowRenderPass.h"
#include "vulkanStorageBuffer.h"
#include "vulkanStorageSampleTexture2d.h"
#include "vulkanStorageSampleTexture3d.h"
#include "vulkanStorageTexture2d.h"
#include "vulkanStorageTexture3d.h"
#include "vulkanSurface.h"
#include "vulkanSwapchain.h"
#include "vulkanVertexBuffer.h"
#include <array>
#include <assert.h>
#include <algorithm>
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
		m_pendingMeshUpdates.resize(createInfo.framesInFlight); // Prepare one pending mesh update vector per frame in flight.
		Context::Init(createInfo, pIWindow, this);

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
		m_maxDirectionalLights = math::Clamp(createInfo.maxDirectionalLights, uint32_t(1), uint32_t(MAX_DIR_LIGHTS));
		m_maxPositionalLights = math::Clamp(createInfo.maxPositionalLights, uint32_t(1), uint32_t(MAX_POS_LIGHTS));
		m_shadowMapResolution = math::Clamp(createInfo.shadowMapResolution, uint32_t(1), uint32_t(SHADOW_MAP_RESOLUTION));
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

		SortDrawCallPointers();
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

			RecordShadowCommands();
			SubmitShadowCommands();

			RecordForwardCommands();
			SubmitForwardCommands();

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

		// Reset render state:
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
    void Renderer::DrawOutline(emberBackendInterface::IMesh* pIMesh, const Float4x4& localToWorldMatrix, uint32_t instanceCount)
    {
		if (!pIMesh)
		{
			LOG_ERROR("vulkanRendererBackend::Renderer::DrawOutline(...) failed. pMesh is nullptr.");
			return;
		}

		// Setup outline call:
		Material* pMaterial = DefaultGpuResources::GetDefaultOutlineMaterial();
		DescriptorSetBindingHandle descriptorSetBindingHandle = PoolManager::CheckOutCallDescriptorSetBindingHandle(pMaterial->GetShader());
		DrawCall::MaterialState materialState(pMaterial, descriptorSetBindingHandle);
		DrawCall drawCall(localToWorldMatrix, materialState, static_cast<Mesh*>(pIMesh), instanceCount);
		m_outlineCalls.push_back(drawCall);
    }
	void Renderer::DrawMesh(emberBackendInterface::IMesh* pIMesh, emberBackendInterface::IMaterial* pIMaterial, emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows, uint32_t instanceCount)
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

		// Setup draw call:
		Material* pMaterial = static_cast<Material*>(pIMaterial);
		if (castShadows)
		{
			Material* pShadowMaterial = pMaterial->GetShadowMaterial();
			if (!pShadowMaterial)
				pShadowMaterial = DefaultGpuResources::GetDefaultShadowMaterial();
			DescriptorSetBindingHandle descriptorSetBindingHandle(static_cast<DescriptorSetBinding*>(pICallDescriptorSetBinding));
			DescriptorSetBindingHandle shadowDescriptorSetBindingHandle = PoolManager::CheckOutCallDescriptorSetBindingHandle(pShadowMaterial->GetShader());
			DrawCall::MaterialState materialState(pMaterial, descriptorSetBindingHandle);
			DrawCall::ShadowState shadowState(pShadowMaterial, shadowDescriptorSetBindingHandle, receiveShadows, castShadows);
			DrawCall drawCall(localToWorldMatrix, materialState, shadowState, static_cast<Mesh*>(pIMesh), instanceCount);
			m_drawCalls.push_back(drawCall);
		}
		else
		{
			DescriptorSetBindingHandle descriptorSetBindingHandle(static_cast<DescriptorSetBinding*>(pICallDescriptorSetBinding));
			DrawCall::MaterialState materialState(pMaterial, descriptorSetBindingHandle);
			DrawCall drawCall(localToWorldMatrix, materialState, static_cast<Mesh*>(pIMesh), instanceCount);
			m_drawCalls.push_back(drawCall);
		}
	}
	emberBackendInterface::IDescriptorSetBinding* Renderer::DrawMesh(emberBackendInterface::IMesh* pIMesh, emberBackendInterface::IMaterial* pIMaterial, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows, uint32_t instanceCount)
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

		// Setup draw call:
		Material* pMaterial = static_cast<Material*>(pIMaterial);
		DescriptorSetBindingHandle descriptorSetBindingHandle = PoolManager::CheckOutCallDescriptorSetBindingHandle(pMaterial->GetShader());
		if (castShadows)
		{
			Material* pShadowMaterial = pMaterial->GetShadowMaterial();
			if (!pShadowMaterial)
				pShadowMaterial = DefaultGpuResources::GetDefaultShadowMaterial();
			DescriptorSetBindingHandle shadowDescriptorSetBindingHandle = PoolManager::CheckOutCallDescriptorSetBindingHandle(pShadowMaterial->GetShader());
			DrawCall::MaterialState materialState(pMaterial, descriptorSetBindingHandle);
			DrawCall::ShadowState shadowState(pShadowMaterial, shadowDescriptorSetBindingHandle, receiveShadows, true);
			DrawCall drawCall(localToWorldMatrix, materialState, shadowState, static_cast<Mesh*>(pIMesh), instanceCount);
			m_drawCalls.push_back(drawCall);
		}
		else
		{
			DrawCall::MaterialState materialState(pMaterial, descriptorSetBindingHandle);
			DrawCall drawCall(localToWorldMatrix, materialState, static_cast<Mesh*>(pIMesh), instanceCount);
			m_drawCalls.push_back(drawCall);
		}
		return descriptorSetBindingHandle.Get();
	}
	void Renderer::DrawGizmo(emberBackendInterface::IMesh* pIMesh, emberBackendInterface::IMaterial* pIMaterial, emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding, const Float4x4& localToWorldMatrix, uint32_t instanceCount)
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

		// Setup draw call:
		Material* pMaterial = static_cast<Material*>(pIMaterial);
		DescriptorSetBindingHandle descriptorSetBindingHandle(static_cast<DescriptorSetBinding*>(pICallDescriptorSetBinding));
		DrawCall::MaterialState materialState(pMaterial, descriptorSetBindingHandle);
		DrawCall drawCall(localToWorldMatrix, materialState, static_cast<Mesh*>(pIMesh), instanceCount);
		m_gizmoDrawCalls.push_back(drawCall);
	}
	emberBackendInterface::IDescriptorSetBinding* Renderer::DrawGizmo(emberBackendInterface::IMesh* pIMesh, emberBackendInterface::IMaterial* pIMaterial, const Float4x4& localToWorldMatrix, uint32_t instanceCount)
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

		// Setup draw call:
		Material* pMaterial = static_cast<Material*>(pIMaterial);
		DescriptorSetBindingHandle descriptorSetBindingHandle = PoolManager::CheckOutCallDescriptorSetBindingHandle(pMaterial->GetShader());
		DrawCall::MaterialState materialState(pMaterial, descriptorSetBindingHandle);
		DrawCall drawCall(localToWorldMatrix, materialState, static_cast<Mesh*>(pIMesh), instanceCount);
		m_gizmoDrawCalls.push_back(drawCall);
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
	emberBackendInterface::ITexture* Renderer::GetRenderTexture()
	{
		RenderTexture2d* pRenderTexture = RenderPassManager::GetForwardRenderPass()->GetRenderTexture(m_frameIndex);
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
	void Renderer::SetDefaultMaterials(emberBackendInterface::IMaterial* pOutlineMaterial, emberBackendInterface::IMaterial* pDefaultShadowMaterial, emberBackendInterface::IMaterial* pPresentMaterial)
	{
		DefaultGpuResources::SetDefaultMaterials(pOutlineMaterial, pDefaultShadowMaterial, pPresentMaterial);
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



	// Gpu resource factories:
	emberBackendInterface::IBuffer* Renderer::CreateBuffer(uint32_t count, uint32_t elementSize, emberCommon::BufferUsage usage)
	{
		emberBackendInterface::IBuffer* pIBuffer = nullptr;
		switch (usage)
		{
		case emberCommon::BufferUsage::index:
			pIBuffer = new IndexBuffer(count, elementSize);
			break;
		case emberCommon::BufferUsage::storage:
			pIBuffer = new StorageBuffer(count, elementSize);
			break;
		case emberCommon::BufferUsage::vertex:
			pIBuffer = new VertexBuffer(count, elementSize);
			break;
		default:
			throw std::runtime_error("vulkanRendererBackend::Renderer::CreateBuffer: Unknown invalid BufferUsage type: " + std::string(emberCommon::BufferUsageToString(usage)));
		}
		return pIBuffer;
	}
	//emberBackendInterface::ITexture* Renderer::CreateTexture1d(int width, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage)
	//{
	//
	//}
	emberBackendInterface::ITexture* Renderer::CreateTexture2d(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data)
	{
		VkFormat vulkanFormat = TextureFormatCommonToVulkan(format);
		emberBackendInterface::ITexture* pITexture = nullptr;
		switch (usage)
		{
		case emberCommon::TextureUsage::sample:
			pITexture = new SampleTexture2d(vulkanFormat, width, height, data);
			break;
		case emberCommon::TextureUsage::storage:
			pITexture = new StorageTexture2d(vulkanFormat, width, height, data);
			break;
		case emberCommon::TextureUsage::storageSample:
			pITexture = new StorageSampleTexture2d(vulkanFormat, width, height, data);
			break;
		case emberCommon::TextureUsage::renderTarget:
			pITexture = new RenderTexture2d(vulkanFormat, width, height);
			break;
		default:
			throw std::runtime_error("vulkanRendererBackend::Renderer::CreateTexture2d: invalid TextureUsage type: " + std::string(emberCommon::TextureUsageToString(usage)));
		}
		return pITexture;
	}
	emberBackendInterface::ITexture* Renderer::CreateTexture3d(int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data)
	{
		VkFormat vulkanFormat = TextureFormatCommonToVulkan(format);
		emberBackendInterface::ITexture* pITexture = nullptr;
		switch (usage)
		{
		case emberCommon::TextureUsage::sample:
			pITexture = new SampleTexture3d(vulkanFormat, width, height, depth, data);
			break;
		case emberCommon::TextureUsage::storage:
			pITexture = new StorageTexture3d(vulkanFormat, width, height, depth, data);
			break;
		case emberCommon::TextureUsage::storageSample:
			pITexture = new StorageSampleTexture3d(vulkanFormat, width, height, depth, data);
			break;
		default:
			throw std::runtime_error("vulkanRendererBackend::Renderer::CreateTexture3d: unsupported TextureUsage type: " + std::string(emberCommon::TextureUsageToString(usage)));
		}
		return pITexture;
	}
	emberBackendInterface::ITexture* Renderer::CreateTextureCube(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data)
	{
		VkFormat vulkanFormat = TextureFormatCommonToVulkan(format);
		emberBackendInterface::ITexture* pITexture = nullptr;
		switch (usage)
		{
		case emberCommon::TextureUsage::sample:
			pITexture = new SampleTextureCube(vulkanFormat, width, height, data);
			break;
		default:
			throw std::runtime_error("vulkanRendererBackend::Renderer::CreateTextureCube: invalid TextureUsage type: " + std::string(emberCommon::TextureUsageToString(usage)));
		}
		return pITexture;
	}
	emberBackendInterface::IComputeShader* Renderer::CreateComputeShader(const std::filesystem::path& computeSpv, const std::string& debugName)
	{
		return new ComputeShader(computeSpv, debugName);
	}
	emberBackendInterface::IMaterialShader* Renderer::CreateOutlineMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName)
	{
		return new MaterialShader(MaterialShader::CreateOutline(vertexSpv, fragmentSpv, debugName));
	}
	emberBackendInterface::IMaterialShader* Renderer::CreateForwardMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName)
	{
		return new MaterialShader(MaterialShader::CreateForward(vertexSpv, fragmentSpv, debugName));
	}
	emberBackendInterface::IMaterialShader* Renderer::CreateGizmoMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName)
	{
		return new MaterialShader(MaterialShader::CreateGizmo(vertexSpv, fragmentSpv, debugName));
	}
	emberBackendInterface::IMaterialShader* Renderer::CreateShadowMaterialShader(const std::filesystem::path& vertexSpv, const std::string& debugName)
	{
		return new MaterialShader(MaterialShader::CreateShadow(m_shadowMapResolution, vertexSpv, debugName));
	}
	emberBackendInterface::IMaterialShader* Renderer::CreatePresentMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName)
	{
		return new MaterialShader(MaterialShader::CreatePresent(vertexSpv, fragmentSpv, debugName));
	}
	emberBackendInterface::IMaterial* Renderer::CreateOutlineMaterial(emberBackendInterface::IMaterialShader* pMaterialShader, const std::string& debugName)
	{
		if (pMaterialShader == nullptr)
			throw std::runtime_error("Renderer::CreateOutlineMaterial(...) failed. MaterialShader is null.");
		if (pMaterialShader->GetMaterialType() != emberCommon::MaterialType::outline)
			throw std::runtime_error("Renderer::CreateOutlineMaterial(...) failed. MaterialShader is not an outline shader.");

		MaterialShader* pVulkanMaterialShader = static_cast<MaterialShader*>(pMaterialShader);
		return new Material(Material::CreateOutline(pVulkanMaterialShader, debugName));
	}
	emberBackendInterface::IMaterial* Renderer::CreateForwardMaterial(emberCommon::ForwardRenderMode renderMode, emberBackendInterface::IMaterialShader* pMaterialShader, const std::string& debugName)
	{
		if (pMaterialShader == nullptr)
			throw std::runtime_error("Renderer::CreateForwardMaterial(...) failed. MaterialShader is null.");
		if (pMaterialShader->GetMaterialType() != emberCommon::MaterialType::forward)
			throw std::runtime_error("Renderer::CreateForwardMaterial(...) failed. MaterialShader is not a forward shader.");

		MaterialShader* pVulkanMaterialShader = static_cast<MaterialShader*>(pMaterialShader);
		return new Material(Material::CreateForward(pVulkanMaterialShader, renderMode, debugName));
	}
	emberBackendInterface::IMaterial* Renderer::CreateGizmoMaterial(emberCommon::GizmoRenderMode renderMode, emberBackendInterface::IMaterialShader* pMaterialShader, const std::string& debugName)
	{
		if (pMaterialShader == nullptr)
			throw std::runtime_error("Renderer::CreateGizmoMaterial(...) failed. MaterialShader is null.");
		if (pMaterialShader->GetMaterialType() != emberCommon::MaterialType::gizmo)
			throw std::runtime_error("Renderer::CreateGizmoMaterial(...) failed. MaterialShader is not a gizmo shader.");

		MaterialShader* pVulkanMaterialShader = static_cast<MaterialShader*>(pMaterialShader);
		return new Material(Material::CreateGizmo(pVulkanMaterialShader, renderMode, debugName));
	}
	emberBackendInterface::IMaterial* Renderer::CreateShadowMaterial(emberBackendInterface::IMaterialShader* pMaterialShader, const std::string& debugName)
	{
		if (pMaterialShader == nullptr)
			throw std::runtime_error("Renderer::CreateShadowMaterial(...) failed. MaterialShader is null.");
		if (pMaterialShader->GetMaterialType() != emberCommon::MaterialType::shadow)
			throw std::runtime_error("Renderer::CreateShadowMaterial(...) failed. MaterialShader is not a shadow shader.");

		MaterialShader* pVulkanMaterialShader = static_cast<MaterialShader*>(pMaterialShader);
		return new Material(Material::CreateShadow(pVulkanMaterialShader, debugName));
	}
	emberBackendInterface::IMaterial* Renderer::CreatePresentMaterial(emberBackendInterface::IMaterialShader* pMaterialShader, const std::string& debugName)
	{
		if (pMaterialShader == nullptr)
			throw std::runtime_error("Renderer::CreatePresentMaterial(...) failed. MaterialShader is null.");
		if (pMaterialShader->GetMaterialType() != emberCommon::MaterialType::present)
			throw std::runtime_error("Renderer::CreatePresentMaterial(...) failed. MaterialShader is not a present shader.");

		MaterialShader* pVulkanMaterialShader = static_cast<MaterialShader*>(pMaterialShader);
		return new Material(Material::CreatePresent(pVulkanMaterialShader, debugName));
	}
	emberBackendInterface::IMaterial* Renderer::CloneForwardMaterial(emberBackendInterface::IMaterial* pSourceMaterial, const std::string& debugName)
	{
		return new Material(Material::CloneForward(*static_cast<Material*>(pSourceMaterial), debugName));
	}
	emberBackendInterface::IMaterial* Renderer::CloneGizmoMaterial(emberBackendInterface::IMaterial* pSourceMaterial, const std::string& debugName)
	{
		return new Material(Material::CloneGizmo(*static_cast<Material*>(pSourceMaterial), debugName));
	}
	emberBackendInterface::IMaterial* Renderer::CloneShadowMaterial(emberBackendInterface::IMaterial* pSourceMaterial, const std::string& debugName)
	{
		return new Material(Material::CloneShadow(*static_cast<Material*>(pSourceMaterial), debugName));
	}
	emberBackendInterface::IMesh* Renderer::CreateMesh()
	{
		return new Mesh();
	}
	emberBackendInterface::IDescriptorSetBinding* Renderer::CreateDrawCallDescriptorSetBinding(emberBackendInterface::IMaterial* pIMaterial)
	{
		Material* pMaterial = static_cast<Material*>(pIMaterial);
		Shader* pShader = pMaterial->GetShader();
		return new DescriptorSetBinding(pShader, CALL_SET_INDEX, pMaterial->GetDebugName());
	}


	
	// Gpu resource destruction:
	void Renderer::DestroyMaterial(emberBackendInterface::IMaterial* pMaterial)
	{
		if (!pMaterial)
			return;

		Material* pVulkanMaterial = static_cast<Material*>(pMaterial);
		GarbageCollector::RecordGarbage([pVulkanMaterial]()
		{
			delete pVulkanMaterial;
		});
	}
	void Renderer::DestroyMaterialShader(emberBackendInterface::IMaterialShader* pMaterialShader)
	{
		if (!pMaterialShader)
			return;

		MaterialShader* pVulkanMaterialShader = static_cast<MaterialShader*>(pMaterialShader);
		GarbageCollector::RecordGarbage([pVulkanMaterialShader]()
		{
			delete pVulkanMaterialShader;
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
		// Return all borrowed descriptor set bindings borrowed to the corresponding pool:
        for (DrawCall& drawCall: m_outlineCalls)
			PoolManager::ReturnCallDescriptorSetBinding(drawCall.materialState.descriptorSetBindingHandle);
		for (DrawCall& drawCall : m_drawCalls)
		{
			PoolManager::ReturnCallDescriptorSetBinding(drawCall.materialState.descriptorSetBindingHandle);
			PoolManager::ReturnCallDescriptorSetBinding(drawCall.shadowState.descriptorSetBindingHandle);
		}
		for (DrawCall& drawCall : m_gizmoDrawCalls)
			PoolManager::ReturnCallDescriptorSetBinding(drawCall.materialState.descriptorSetBindingHandle);

		// Clear all draw calls for next frame:
		m_outlineCalls.clear();
		m_drawCalls.clear();
		m_sortedDrawCallPointers.clear();
		m_gizmoDrawCalls.clear();
		m_sortedGizmoDrawCallPointers.clear();
	}
	void Renderer::ResetCommandPools()
	{
		GetCommandPool(m_frameIndex, RenderStage::resourceUpdate).ResetPools();
		GetCommandPool(m_frameIndex, RenderStage::preRenderCompute).ResetPools();
		GetCommandPool(m_frameIndex, RenderStage::outline).ResetPools();
		GetCommandPool(m_frameIndex, RenderStage::shadow).ResetPools();
		GetCommandPool(m_frameIndex, RenderStage::forward).ResetPools();
		GetCommandPool(m_frameIndex, RenderStage::gizmo).ResetPools();
		GetCommandPool(m_frameIndex, RenderStage::postRenderCompute).ResetPools();
		GetCommandPool(m_frameIndex, RenderStage::present).ResetPools();
	}



	void Renderer::RebuildSwapchain()
	{
		// Recreate swapchain:
		Context::RebuildSwapchain();	// calls WaitDeviceIdle().

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
		m_sortedDrawCallPointers.clear();
		m_sortedDrawCallPointers.reserve(m_drawCalls.size());
		for (auto& drawCall : m_drawCalls)
			m_sortedDrawCallPointers.push_back(&drawCall);
		m_sortedGizmoDrawCallPointers.clear();
		m_sortedGizmoDrawCallPointers.reserve(m_gizmoDrawCalls.size());
		for (auto& drawCall : m_gizmoDrawCalls)
			m_sortedGizmoDrawCallPointers.push_back(&drawCall);

		// Sort by renderQueue first, then handle transparent draw order, then group by vertex layout.
		auto sortDrawCallPointers = [this](std::vector<DrawCall*>& sortedDrawCallPointers)
		{
			std::sort(sortedDrawCallPointers.begin(), sortedDrawCallPointers.end(), [this](DrawCall* drawCallA, DrawCall* drawCallB)
			{
				// RenderQueue:
				int renderQueueA = static_cast<int>(drawCallA->materialState.pMaterial->GetRenderQueue());
				int renderQueueB = static_cast<int>(drawCallB->materialState.pMaterial->GetRenderQueue());
				if (renderQueueA != renderQueueB)
					return renderQueueA < renderQueueB;

				// Transparent materials back-to-front ordering:
				const bool transparentA = drawCallA->materialState.pMaterial->IsTransparent();
				const bool transparentB = drawCallB->materialState.pMaterial->IsTransparent();
				if (transparentA && transparentB)
				{
					const Float3 drawPositionA = Float3(drawCallA->localToWorldMatrix * Float4(0.0f, 0.0f, 0.0f, 1.0f));
					const Float3 drawPositionB = Float3(drawCallB->localToWorldMatrix * Float4(0.0f, 0.0f, 0.0f, 1.0f));
					const float distanceA = Float3::DistanceSq(drawPositionA, m_activeCamera.position);
					const float distanceB = Float3::DistanceSq(drawPositionB, m_activeCamera.position);
					if (distanceA != distanceB)
						return distanceA > distanceB;
				}

				// Interleaved(0) before separate(1):
				auto layoutA = drawCallA->pMesh->GetVertexMemoryLayout();
				auto layoutB = drawCallB->pMesh->GetVertexMemoryLayout();
				if (layoutA != layoutB)
					return layoutA < layoutB;

				// Tie-breaker by pointer:
				return drawCallA < drawCallB;
			});
		};
		sortDrawCallPointers(m_sortedDrawCallPointers);
		sortDrawCallPointers(m_sortedGizmoDrawCallPointers);
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
			if (computeCall.pComputeShader)
			{
				computeCall.pComputeShader->GetDescriptorSetBinding()->UpdateShaderData(m_frameIndex);
				computeCall.callDescriptorSetBindingHandle.Get()->UpdateShaderData(m_frameIndex);
			}

		// Outline calls:
		if (!m_outlineCalls.empty())
			DefaultGpuResources::GetDefaultOutlineMaterial()->GetDescriptorSetBinding()->UpdateShaderData(m_frameIndex);
		for (DrawCall& drawCall : m_outlineCalls)
		{
			drawCall.UpdateModelData();
			drawCall.materialState.descriptorSetBindingHandle.Get()->UpdateShaderData(m_frameIndex);
		}

		// Forward calls:
		for (DrawCall* drawCall : m_sortedDrawCallPointers)
		{
			drawCall->UpdateModelData();
			drawCall->materialState.pMaterial->GetDescriptorSetBinding()->UpdateShaderData(m_frameIndex);
			drawCall->materialState.descriptorSetBindingHandle.Get()->UpdateShaderData(m_frameIndex);
			if (drawCall->shadowState.pMaterial)
				drawCall->shadowState.pMaterial->GetDescriptorSetBinding()->UpdateShaderData(m_frameIndex);
			if (drawCall->shadowState.descriptorSetBindingHandle.IsValid())
				drawCall->shadowState.descriptorSetBindingHandle.Get()->UpdateShaderData(m_frameIndex);
		}

		// Gizmo calls:
		for (DrawCall* drawCall : m_sortedGizmoDrawCallPointers)
		{
			drawCall->UpdateModelData();
			drawCall->materialState.pMaterial->GetDescriptorSetBinding()->UpdateShaderData(m_frameIndex);
			drawCall->materialState.descriptorSetBindingHandle.Get()->UpdateShaderData(m_frameIndex);
		}

		// Post render compute:
		PostRender* pPostRenderCompute = m_pCompute->GetPostRenderCompute();
		if (!m_outlineCalls.empty())
		{
			OutlineRenderPass* pOutlineRenderPass = RenderPassManager::GetOutlineRenderPass();
			ComputeShader* pOutlineComputeShader = DefaultGpuResources::GetOutlineComputeShader();
			DescriptorSetBinding* pOutlineDescriptorSetBinding = pOutlineComputeShader->GetDescriptorSetBinding();
			ForwardRenderPass* pForwardRenderPass = RenderPassManager::GetForwardRenderPass();
			RenderTexture2d* pRenderTexture = pPostRenderCompute->GetPostProcessingCallCount() % 2 == 0 ? pForwardRenderPass->GetRenderTexture(m_frameIndex) : pForwardRenderPass->GetSecondaryRenderTexture(m_frameIndex);
			pOutlineDescriptorSetBinding->SetTexture("renderImage", pRenderTexture);
			pOutlineDescriptorSetBinding->SetTexture("mask", pOutlineRenderPass->GetRenderTexture(m_frameIndex));
			pOutlineDescriptorSetBinding->SetFloat4("OutlineProperties", "outlineColor", m_outlineColor);
			pOutlineDescriptorSetBinding->SetInt("OutlineProperties", "outlineRadius", m_outlineThickness);
			pPostRenderCompute->RecordComputeShader(pOutlineComputeShader);
		}

		// We use linear color space throughout the render process. Apply gamma correction as the final post-render operation:
		pPostRenderCompute->RecordPostProcessingShader(DefaultGpuResources::GetGammaCorrectionComputeShader());

		size_t postProcessingCallIndex = 0;
		for (ComputeCall& computeCall : pPostRenderCompute->GetComputeCalls())
		{
			if (computeCall.isPostProcessing)
			{
				if (postProcessingCallIndex % 2 == 0)
				{
					computeCall.callDescriptorSetBindingHandle.Get()->SetTexture("inputImage", RenderPassManager::GetForwardRenderPass()->GetRenderTexture(m_frameIndex));
					computeCall.callDescriptorSetBindingHandle.Get()->SetTexture("outputImage", RenderPassManager::GetForwardRenderPass()->GetSecondaryRenderTexture(m_frameIndex));
				}
				else
				{
					computeCall.callDescriptorSetBindingHandle.Get()->SetTexture("inputImage", RenderPassManager::GetForwardRenderPass()->GetSecondaryRenderTexture(m_frameIndex));
					computeCall.callDescriptorSetBindingHandle.Get()->SetTexture("outputImage", RenderPassManager::GetForwardRenderPass()->GetRenderTexture(m_frameIndex));
				}
				postProcessingCallIndex++;
			}
			computeCall.pComputeShader->GetDescriptorSetBinding()->UpdateShaderData(m_frameIndex);
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
		commandPool.ResetPools();
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
				for (DrawCall* drawCall : m_sortedGizmoDrawCallPointers)
				{
					// Pipeline swap:
					Material* pGizmoMaterial = drawCall->materialState.pMaterial;
					VkPipeline newPipeline = pGizmoMaterial->GetPipeline<RenderStage::gizmo>(drawCall->pMesh)->GetVkPipeline();
					bool pipelineLayoutChanged = false;
					if (pipeline != newPipeline)
					{
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
					vkCmdSetCullMode(commandBuffer, CullModeCommonToVulkan(pGizmoMaterial->GetCullMode()));

					// Bind per draw call descriptor set:
					if (VkDescriptorSet vkDescriptorSet = drawCall->materialState.descriptorSetBindingHandle.Get()->GetVkDescriptorSet(m_frameIndex); vkDescriptorSet != VK_NULL_HANDLE)
						vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, CALL_SET_INDEX, 1, &vkDescriptorSet, 0, nullptr);

					// Bind mesh data:
					vkCmdBindVertexBuffers(commandBuffer, 0, drawCall->pMesh->GetVertexBindingCount(), drawCall->pMesh->GetVkBuffers(), drawCall->pMesh->GetOffsets());
					vkCmdBindIndexBuffer(commandBuffer, drawCall->pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, drawCall->pMesh->GetVkIndexType());

					// Draw call:
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
			if (computeCalls.size() > 0)
			{
				// Pipeline:
				VkPipeline pipeline = VK_NULL_HANDLE;
				VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
				bool staticDescriptorSetsBound = false;

				for (size_t computeCallIndex = 0; computeCallIndex < computeCalls.size(); computeCallIndex++)
				{
					ComputeCall* computeCall = &computeCalls[computeCallIndex];
					ComputeShader* pComputeShader = computeCall->pComputeShader;

					// Compute call is a barrier:
					if (pComputeShader == nullptr)
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
						VkPipeline newPipeline = pComputeShader->GetPipeline()->GetVkPipeline();
						if (pipeline != newPipeline)
						{
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
		commandPool.ResetPools();
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
				for (const DrawCall& drawCall : m_outlineCalls)
				{
					// Pipeline swap:
					Material* pOutlineMaterial = drawCall.materialState.pMaterial;
					VkPipeline newPipeline = pOutlineMaterial->GetPipeline<RenderStage::outline>(drawCall.pMesh)->GetVkPipeline();
					bool pipelineLayoutChanged = false;
					if (pipeline != newPipeline)
					{
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
					if (VkDescriptorSet vkDescriptorSet = drawCall.materialState.descriptorSetBindingHandle.Get()->GetVkDescriptorSet(m_frameIndex); vkDescriptorSet != VK_NULL_HANDLE)
						vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, CALL_SET_INDEX, 1, &vkDescriptorSet, 0, nullptr);

					// Bind mesh data:
					vkCmdBindVertexBuffers(commandBuffer, 0, drawCall.pMesh->GetVertexBindingCount(), drawCall.pMesh->GetVkBuffers(), drawCall.pMesh->GetOffsets());
					vkCmdBindIndexBuffer(commandBuffer, drawCall.pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, drawCall.pMesh->GetVkIndexType());

					// Draw call:
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
			if (m_sortedDrawCallPointers.size() > 0)
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
					for (DrawCall* drawCall : m_sortedDrawCallPointers)
					{
						if (drawCall->shadowState.castShadows == false)
							continue;

						// Pipeline swap:
						const Material* pShadowMaterial = drawCall->shadowState.pMaterial;
						VkPipeline newPipeline = pShadowMaterial->GetPipeline<RenderStage::shadow>(drawCall->pMesh)->GetVkPipeline();
						bool pipelineLayoutChanged = false;
						if (pipeline != newPipeline)
						{
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
						if (VkDescriptorSet vkDescriptorSet = drawCall->shadowState.descriptorSetBindingHandle.Get()->GetVkDescriptorSet(m_frameIndex); vkDescriptorSet != VK_NULL_HANDLE)
							vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, CALL_SET_INDEX, 1, &vkDescriptorSet, 0, nullptr);

						// Bind mesh data:
						vkCmdBindVertexBuffers(commandBuffer, 0, drawCall->pMesh->GetVertexBindingCount(), drawCall->pMesh->GetVkBuffers(), drawCall->pMesh->GetOffsets());
						vkCmdBindIndexBuffer(commandBuffer, drawCall->pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, drawCall->pMesh->GetVkIndexType());

						for (uint32_t shadowMapIndex = 0; shadowMapIndex < shadowLightCount; shadowMapIndex++)
						{
							// Push constant:
							DefaultPushConstant pushConstant(shadowMapIndex, drawCall->instanceCount, false, m_time, m_deltaTime);
							vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(DefaultPushConstant), &pushConstant);

							// Dispatch:
							vkCmdDrawIndexed(commandBuffer, drawCall->pMesh->GetIndexCount(), std::max(drawCall->instanceCount, (uint32_t)1), 0, 0, 0);
							DEBUG_LOG_INFO("Light {}, mesh = {}", shadowMapIndex, drawCall->pMesh->GetName());
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
	void Renderer::RecordForwardCommands()
	{
		PROFILE_FUNCTION();

		// Prepare command recording:
		CommandPool& commandPool = GetCommandPool(m_frameIndex, RenderStage::forward);
		commandPool.ResetPools();
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		ForwardRenderPass* pForwardRenderPass = RenderPassManager::GetForwardRenderPass();

		// Record forward commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			// Viewport and scissor:
			VkViewport viewport = {};
			viewport.width = pForwardRenderPass->GetRenderTexture(m_frameIndex)->GetWidth();
			viewport.height = pForwardRenderPass->GetRenderTexture(m_frameIndex)->GetHeight();
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
			renderPassBeginInfo.renderPass = pForwardRenderPass->GetVkRenderPass();
			renderPassBeginInfo.framebuffer = pForwardRenderPass->GetFramebuffer(m_frameIndex);
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent.width = viewport.width;
			renderPassBeginInfo.renderArea.extent.height = viewport.height;
			renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassBeginInfo.pClearValues = clearValues.data();

			// Begin render pass:
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			if (m_sortedDrawCallPointers.size() > 0)
			{
				// Pipeline:
				VkPipeline pipeline = VK_NULL_HANDLE;
				VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
				VkDescriptorSet shaderDescriptorSet = VK_NULL_HANDLE;
				bool staticDescriptorSetsBound = false;

				// Draw calls:
				for (DrawCall* drawCall : m_sortedDrawCallPointers)
				{
					// Pipeline swap:
					Material* pForwardMaterial = drawCall->materialState.pMaterial;
					VkPipeline newPipeline = pForwardMaterial->GetPipeline<RenderStage::forward>(drawCall->pMesh)->GetVkPipeline();
					bool pipelineLayoutChanged = false;
					if (pipeline != newPipeline)
					{
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
					DefaultPushConstant pushConstant(0, drawCall->instanceCount, drawCall->shadowState.receiveShadows, m_time, m_deltaTime);
					vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(DefaultPushConstant), &pushConstant);

					// Cull mode:
					vkCmdSetCullMode(commandBuffer, CullModeCommonToVulkan(pForwardMaterial->GetCullMode()));

					// Bind per draw call descriptor set:
					if (VkDescriptorSet vkDescriptorSet = drawCall->materialState.descriptorSetBindingHandle.Get()->GetVkDescriptorSet(m_frameIndex); vkDescriptorSet != VK_NULL_HANDLE)
						vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, CALL_SET_INDEX, 1, &vkDescriptorSet, 0, nullptr);
					
					// Bind mesh data:
					vkCmdBindVertexBuffers(commandBuffer, 0, drawCall->pMesh->GetVertexBindingCount(), drawCall->pMesh->GetVkBuffers(), drawCall->pMesh->GetOffsets());
					vkCmdBindIndexBuffer(commandBuffer, drawCall->pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, drawCall->pMesh->GetVkIndexType());
					
					// Draw call:
					vkCmdDrawIndexed(commandBuffer, drawCall->pMesh->GetIndexCount(), std::max(drawCall->instanceCount, (uint32_t)1), 0, 0, 0);
					DEBUG_LOG_TRACE("Forward draw call, mesh = {}, material = {}", drawCall->pMesh->GetName(), pForwardMaterial->GetDebugName());
				}
			}
			vkCmdEndRenderPass(commandBuffer);

			// Make the forward color attachment available to post render compute shaders:
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
				DEBUG_LOG_TRACE("Memory Barrier: forward color attachment to post compute");
			}
		}
		VKA(vkEndCommandBuffer(commandBuffer));

		// Forward render pass's color resolve finalLayout is VK_IMAGE_LAYOUT_GENERAL. Reflect this in the image layout:
		pForwardRenderPass->GetRenderTexture(m_frameIndex)->GetVmaImage()->SetLayout(VK_IMAGE_LAYOUT_GENERAL);
	}
	void Renderer::RecordForwardCommandsParallel()
	{
		//PROFILE_FUNCTION();
		//
		//// Logic for workload splitting across threads:
		//int totalWorkload = (int)m_sortedDrawCallPointers.size();
		//int threadIndex = emberTaskSystem::TaskSystem::GetThreadIndex();
		//int coreCount = emberTaskSystem::TaskSystem::GetCoreCount();
		//int baseChunkSize = totalWorkload / coreCount;
		//int remainder = totalWorkload % coreCount;
		//int startIndex = threadIndex * baseChunkSize + std::min(threadIndex, remainder);
		//int endIndex = startIndex + baseChunkSize + (threadIndex < remainder ? 1 : 0);
		//
		//// Prepare command recording:
		//CommandPool& commandPool = GetCommandPool(m_frameIndex, RenderStage::forward);
		//VkCommandBuffer& secondaryCommandBuffer = commandPool.GetSecondaryVkCommandBuffer(threadIndex);
		//
		//VkCommandBufferInheritanceInfo inheritanceInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO };
		//inheritanceInfo.renderPass = RenderPassManager::GetForwardRenderPass()->GetVkRenderPass();
		//inheritanceInfo.framebuffer = RenderPassManager::GetForwardRenderPass()->GetFramebuffer(0);
		//inheritanceInfo.subpass = 0;
		//
		//VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		//beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		//beginInfo.pInheritanceInfo = &inheritanceInfo;
		//
		//// Record forward commands:
		//VKA(vkBeginCommandBuffer(secondaryCommandBuffer, &beginInfo));
		//{
		//	// Viewport and scissor:
		//	VkViewport viewport = {};
		//	viewport.width = RenderPassManager::GetForwardRenderPass()->GetRenderTexture()->GetWidth();
		//	viewport.height = RenderPassManager::GetForwardRenderPass()->GetRenderTexture()->GetHeight();
		//	viewport.minDepth = 0.0f;
		//	viewport.maxDepth = 1.0f;
		//	VkRect2D scissor = {};
		//	scissor.extent.width = viewport.width;
		//	scissor.extent.height = viewport.height;
		//	vkCmdSetViewport(secondaryCommandBuffer, 0, 1, &viewport);
		//	vkCmdSetScissor(secondaryCommandBuffer, 0, 1, &scissor);
		//
		//	// Record commands: (no begin renderpass for secondary command buffers)
		//	{
		//		VkPipeline pipeline = VK_NULL_HANDLE;
		//		DefaultPushConstant pushConstant(0, m_time, m_deltaTime, m_directionalLightsCount, m_positionalLightsCount, m_activeCamera.position);
		//
		//		// Draw calls:
		//		for (int i = startIndex; i < endIndex; i++)
		//		{
		//			DrawCall* drawCall = (m_sortedDrawCallPointers)[i];
		//
		//			// Pipeline swap:
		//			if (pipeline != drawCall->pMaterial->GetPipeline<RenderStage::forward>(drawCall->pMesh)->GetVkPipeline())
		//			{
		//				pipeline = drawCall->pMaterial->GetPipeline<RenderStage::forward>(drawCall->pMesh)->GetVkPipeline();
		//				pushConstant.instanceCount = drawCall->instanceCount;
		//				vkCmdBindPipeline(secondaryCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		//				vkCmdPushConstants(secondaryCommandBuffer, drawCall->pMaterial->GetVkPipelineLayout(); , VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(DefaultPushConstant), &pushConstant);
		//			}
		//
		//			// Same pipeline but different instance Count => update push constants:
		//			if (pushConstant.instanceCount != drawCall->instanceCount)
		//			{
		//				pushConstant.instanceCount = drawCall->instanceCount;
		//				vkCmdPushConstants(secondaryCommandBuffer, drawCall->pMaterial->GetVkPipelineLayout(); , VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(DefaultPushConstant), &pushConstant);
		//			}
		//
		//			vkCmdBindVertexBuffers(secondaryCommandBuffer, 0, drawCall->pMesh->GetVertexBindingCount(), drawCall->pMesh->GetVkBuffers(), drawCall->pMesh->GetOffsets());
		//			vkCmdBindIndexBuffer(secondaryCommandBuffer, drawCall->pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, drawCall->pMesh->GetVkIndexType());
		//
		//			vkCmdBindDescriptorSets(secondaryCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, drawCall->pMaterial->GetVkPipelineLayout(); , 0, 1, &drawCall->pShaderDescriptorSet->GetVkDescriptorSet(m_frameIndex), 0, nullptr);
		//			vkCmdDrawIndexed(secondaryCommandBuffer, drawCall->pMesh->GetIndexCount(), std::max(drawCall->instanceCount, (uint32_t)1), 0, 0, 0);
		//			DEBUG_LOG_WARN("Forward draw call, mesh = {}, material = {}", drawCall->pMesh->GetName(), drawCall->pMaterial->GetDebugName());
		//		}
		//	}
		//}
		//VKA(vkEndCommandBuffer(secondaryCommandBuffer));
		//
		//// Forward render pass's color resolve finalLayout is VK_IMAGE_LAYOUT_GENERAL. Reflect this in the image layout:
		//RenderPassManager::GetForwardRenderPass()->GetRenderTexture()->GetVmaImage()->SetLayout(VK_IMAGE_LAYOUT_GENERAL);
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
			PostRender* pPostRenderCompute = m_pCompute->GetPostRenderCompute();
			std::vector<ComputeCall>& computeCalls = pPostRenderCompute->GetComputeCalls();
			if (computeCalls.size() > 0)
			{
				// Pipeline:
				VkPipeline pipeline = VK_NULL_HANDLE;
				VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
				bool staticDescriptorSetsBound = false;

				for (size_t computeCallIndex = 0; computeCallIndex < computeCalls.size(); computeCallIndex++)
				{
					ComputeCall* computeCall = &computeCalls[computeCallIndex];
					ComputeShader* pComputeShader = computeCall->pComputeShader;

					// Pipeline change:
					VkPipeline newPipeline = pComputeShader->GetPipeline()->GetVkPipeline();
					if (pipeline != newPipeline)
					{
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
					DEBUG_LOG_TRACE("Post Render Compute Shader {}, call = {}", computeCall->pComputeShader->GetDebugName(), computeCallIndex);

					// Post-render compute shaders execute in recorded order and may access the same resources.
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

			// Transition final render textures layout to shaderReadOnly for compositing and do copy to main renderTexture if needed:
			VmaImage* pRenderImage = RenderPassManager::GetForwardRenderPass()->GetRenderTexture(m_frameIndex)->GetVmaImage();
			VmaImage* pSecondaryRenderImage = RenderPassManager::GetForwardRenderPass()->GetSecondaryRenderTexture(m_frameIndex)->GetVmaImage();
			if (pPostRenderCompute->GetPostProcessingCallCount() % 2 == 1)
			{
				pSecondaryRenderImage->TransitionLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_2_TRANSFER_BIT, AccessMasks::ComputeShader::shaderWrite, AccessMasks::Transfer::transferRead);
				pRenderImage->TransitionLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_2_TRANSFER_BIT, AccessMasks::ComputeShader::shaderRead | AccessMasks::ComputeShader::shaderWrite, AccessMasks::Transfer::transferWrite);
				VmaImage::CopyImageToImage(commandBuffer, pSecondaryRenderImage, pRenderImage, Context::GetLogicalDevice()->GetGraphicsQueue());
				pSecondaryRenderImage->TransitionLayout(commandBuffer, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT, AccessMasks::Transfer::transferRead, AccessMasks::ComputeShader::shaderRead | AccessMasks::ComputeShader::shaderWrite);
				pRenderImage->TransitionLayout(commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, AccessMasks::Transfer::transferWrite, AccessMasks::FragmentShader::shaderRead);
				DEBUG_LOG_TRACE("Copied final post render result into the primary render texture");
			}
			else
			{
				VkPipelineStageFlags2 srcStage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
				AccessMask srcAccessMask = AccessMasks::ColorAttachmentOutput::colorAttachmentWrite | AccessMasks::ComputeShader::shaderWrite;
				pRenderImage->TransitionLayout(commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, srcStage, VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, srcAccessMask, AccessMasks::FragmentShader::shaderRead);
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
			DescriptorSetBinding* pPresentShaderDescriptorSetBinding = DefaultGpuResources::GetDefaultPresentMaterial()->GetDescriptorSetBinding();
			pPresentShaderDescriptorSetBinding->SetTexture("renderTexture", RenderPassManager::GetForwardRenderPass()->GetRenderTexture(m_frameIndex));
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
			Mesh* pMesh = DefaultGpuResources::GetDefaultRenderQuad();
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			{
				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pMaterial->GetPipeline<RenderStage::present>(pMesh)->GetVkPipeline());

				// Bind mesh data:
				vkCmdBindVertexBuffers(commandBuffer, 0, pMesh->GetVertexBindingCount(), pMesh->GetVkBuffers(), pMesh->GetOffsets());
				vkCmdBindIndexBuffer(commandBuffer, pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, pMesh->GetVkIndexType());

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
				vkCmdDrawIndexed(commandBuffer, pMesh->GetIndexCount(), 1, 0, 0, 0);
				if (m_pIGui)
					m_pIGui->Render(commandBuffer);
				DEBUG_LOG_INFO("Render renderTexture into fullScreenRenderQuad, material = {}", pMaterial->GetDebugName());
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
		std::array<VkSemaphoreSubmitInfo, 2> signalSemaphoreInfos{};
		signalSemaphoreInfos[0].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		signalSemaphoreInfos[0].semaphore = m_preRenderComputeToShadowSemaphores[m_frameIndex];
		signalSemaphoreInfos[0].stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
		signalSemaphoreInfos[1].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		signalSemaphoreInfos[1].semaphore = m_preRenderComputeToOutlineSemaphores[m_frameIndex];
		signalSemaphoreInfos[1].stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;

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
		signalSemaphoreInfo.semaphore = m_outlineToPostRenderComputeSemaphores[m_frameIndex];
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
		signalSemaphoreInfo.semaphore = m_shadowToForwardSemaphores[m_frameIndex];
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
	void Renderer::SubmitForwardCommands()
	{
		CommandPool& commandPool = GetCommandPool(m_frameIndex, RenderStage::forward);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		
		// Wait semaphore info:
		VkSemaphoreSubmitInfo waitSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		waitSemaphoreInfo.semaphore = m_shadowToForwardSemaphores[m_frameIndex];
		// The forward pass also consumes uploaded mesh buffers at index/vertex input and then continues
		// through the rest of graphics. Waiting from index input onward prevents the transfer writes from
		// racing with the first buffer reads in the forward pass.
		waitSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

		// Command buffer info:
		VkCommandBufferSubmitInfo commandBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
		commandBufferInfo.commandBuffer = commandBuffer;

		// Signal semaphore info:
		VkSemaphoreSubmitInfo signalSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		signalSemaphoreInfo.semaphore = m_forwardToPostRenderComputeSemaphores[m_frameIndex];
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
	void Renderer::SubmitForwardCommandsParallel()
	{
		CommandPool& commandPool = GetCommandPool(m_frameIndex, RenderStage::forward);
		VkCommandBuffer& primaryCommandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		std::vector<VkCommandBuffer>& secondaryCommandBuffers = commandPool.GetSecondaryVkCommandBuffers();
		VkCommandBufferBeginInfo primaryBeginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		primaryBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(primaryCommandBuffer, &primaryBeginInfo);
		{
			// Render pass info:
			std::array<VkClearValue, 2> clearValues;
			clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
			clearValues[1].depthStencil = { 1.0f, 0 };
			VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
			renderPassBeginInfo.renderPass = RenderPassManager::GetForwardRenderPass()->GetVkRenderPass();
			renderPassBeginInfo.framebuffer = RenderPassManager::GetForwardRenderPass()->GetFramebuffer(m_frameIndex);
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent.width = RenderPassManager::GetForwardRenderPass()->GetRenderTexture(m_frameIndex)->GetWidth();
			renderPassBeginInfo.renderArea.extent.height = RenderPassManager::GetForwardRenderPass()->GetRenderTexture(m_frameIndex)->GetHeight();
			renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassBeginInfo.pClearValues = clearValues.data();

			// Begin render pass:
			vkCmdBeginRenderPass(primaryCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
			{
				vkCmdExecuteCommands(primaryCommandBuffer, secondaryCommandBuffers.size(), secondaryCommandBuffers.data());
			}
			vkCmdEndRenderPass(primaryCommandBuffer);

			// Release memory from vertex shaders to compute shaders:
			{
				VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
				memoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
				memoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
				memoryBarrier.srcAccessMask = AccessMasks::VertexShader::shaderRead;
				memoryBarrier.dstAccessMask = AccessMasks::ComputeShader::shaderWrite;

				VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
				dependencyInfo.memoryBarrierCount = 1;
				dependencyInfo.pMemoryBarriers = &memoryBarrier;

				vkCmdPipelineBarrier2(primaryCommandBuffer, &dependencyInfo);
			}
		}
		vkEndCommandBuffer(primaryCommandBuffer);

		// Wait semaphore info:
		VkSemaphoreSubmitInfo waitSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		waitSemaphoreInfo.semaphore = m_shadowToForwardSemaphores[m_frameIndex];
		// This path submits the same forward render work through a primary command buffer that executes
		// secondary command buffers. It has the same dependency as SubmitForwardCommands(): uploaded
		// mesh buffers must be visible before index/vertex input starts reading them.
		waitSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

		// Command buffer info:
		VkCommandBufferSubmitInfo commandBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
		commandBufferInfo.commandBuffer = primaryCommandBuffer;

		// Signal semaphore info:
		VkSemaphoreSubmitInfo signalSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		signalSemaphoreInfo.semaphore = m_forwardToPostRenderComputeSemaphores[m_frameIndex];
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
		VKA(vkQueueSubmit2(Context::GetLogicalDevice()->GetGraphicsQueue().queue, 1, &submitInfo, VK_NULL_HANDLE));
	}
	void Renderer::SubmitPostRenderComputeCommands()
	{
		CommandPool& commandPool = GetCommandPool(m_frameIndex, RenderStage::postRenderCompute);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();

		// Wait semaphore info:
		std::array<VkSemaphoreSubmitInfo, 2> waitSemaphoreInfos{};
		waitSemaphoreInfos[0].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		waitSemaphoreInfos[0].semaphore = m_forwardToPostRenderComputeSemaphores[m_frameIndex];
		waitSemaphoreInfos[0].stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
		waitSemaphoreInfos[1].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		waitSemaphoreInfos[1].semaphore = m_outlineToPostRenderComputeSemaphores[m_frameIndex];
		waitSemaphoreInfos[1].stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;

		// Command buffer info:
		VkCommandBufferSubmitInfo commandBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
		commandBufferInfo.commandBuffer = commandBuffer;

		// Signal semaphore info:
        // Post compute can have a copy at the end in case of odd number of post processing effects => transferBit
		VkSemaphoreSubmitInfo signalSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		signalSemaphoreInfo.semaphore = m_postRenderToPresentSemaphores[m_frameIndex];
		signalSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_2_TRANSFER_BIT;

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
		waitSemaphoreInfos[0].semaphore = m_postRenderToPresentSemaphores[m_frameIndex];
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
		m_preRenderComputeToOutlineSemaphores.resize(Context::GetFramesInFlight());
		m_shadowToForwardSemaphores.resize(Context::GetFramesInFlight());
		m_forwardToPostRenderComputeSemaphores.resize(Context::GetFramesInFlight());
		m_outlineToPostRenderComputeSemaphores.resize(Context::GetFramesInFlight());
		m_gizmoToPresentSemaphores.resize(Context::GetFramesInFlight());
		m_postRenderToPresentSemaphores.resize(Context::GetFramesInFlight());
		m_releaseSemaphores.resize(Context::GetSwapchain()->GetImageCount());
		for (uint32_t i = 0; i < Context::GetFramesInFlight(); i++)
		{
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_acquireSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_resourceUpdateToPreRenderComputeSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_resourceUpdateToGizmoSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_preRenderComputeToShadowSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_preRenderComputeToOutlineSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_shadowToForwardSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_forwardToPostRenderComputeSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_outlineToPostRenderComputeSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_gizmoToPresentSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_postRenderToPresentSemaphores[i]));
			NAME_VK_OBJECT(m_acquireSemaphores[i], "Semaphore_Acquire_Frame" + std::to_string(i));
			NAME_VK_OBJECT(m_resourceUpdateToPreRenderComputeSemaphores[i], "Semaphore_ResourceUpdateToPreRenderCompute_Frame" + std::to_string(i));
			NAME_VK_OBJECT(m_resourceUpdateToGizmoSemaphores[i], "Semaphore_ResourceUpdateToGizmo_Frame" + std::to_string(i));
			NAME_VK_OBJECT(m_preRenderComputeToShadowSemaphores[i], "Semaphore_PreRenderComputeToShadow_Frame" + std::to_string(i));
			NAME_VK_OBJECT(m_preRenderComputeToOutlineSemaphores[i], "Semaphore_PreRenderComputeToOutline_Frame" + std::to_string(i));
			NAME_VK_OBJECT(m_shadowToForwardSemaphores[i], "Semaphore_ShadowToForward_Frame" + std::to_string(i));
			NAME_VK_OBJECT(m_forwardToPostRenderComputeSemaphores[i], "Semaphore_ForwardToPostRenderCompute_Frame" + std::to_string(i));
			NAME_VK_OBJECT(m_outlineToPostRenderComputeSemaphores[i], "Semaphore_OutlineToPostRenderCompute_Frame" + std::to_string(i));
			NAME_VK_OBJECT(m_gizmoToPresentSemaphores[i], "Semaphore_GizmoToPresent_Frame" + std::to_string(i));
			NAME_VK_OBJECT(m_postRenderToPresentSemaphores[i], "Semaphore_PostRenderToPresent_Frame" + std::to_string(i));
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
			vkDestroySemaphore(Context::GetVkDevice(), m_preRenderComputeToOutlineSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_shadowToForwardSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_forwardToPostRenderComputeSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_outlineToPostRenderComputeSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_gizmoToPresentSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_postRenderToPresentSemaphores[i], nullptr);
		}
		for (uint32_t i = 0; i < m_releaseSemaphores.size(); i++)
			vkDestroySemaphore(Context::GetVkDevice(), m_releaseSemaphores[i], nullptr);
		m_acquireSemaphores.clear();
		m_resourceUpdateToPreRenderComputeSemaphores.clear();
		m_resourceUpdateToGizmoSemaphores.clear();
		m_preRenderComputeToShadowSemaphores.clear();
		m_preRenderComputeToOutlineSemaphores.clear();
		m_shadowToForwardSemaphores.clear();
		m_forwardToPostRenderComputeSemaphores.clear();
		m_outlineToPostRenderComputeSemaphores.clear();
		m_gizmoToPresentSemaphores.clear();
		m_postRenderToPresentSemaphores.clear();
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