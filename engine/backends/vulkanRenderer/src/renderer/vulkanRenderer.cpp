#include "vulkanRenderer.h"
#include "emberMath.h"
#include "iGui.h"
#include "iWindow.h"
#include "logger.h"
#include "profiler.h"
#include "taskSystem.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanAccessMask.h"
#include "vulkanCommandPool.h"
#include "vulkanCompute.h"
#include "vulkanComputeCall.h"
#include "vulkanComputePushConstant.h"
#include "vulkanComputeShader.h"
#include "vulkanContext.h"
#include "vulkanConvertTextureFormat.h"
#include "vulkanDefaultGpuResources.h"
#include "vulkanDefaultPushConstant.h"
#include "vulkanDrawCall.h"
#include "vulkanEngineSet.h"
#include "vulkanForwardRenderPass.h"
#include "vulkanGarbageCollector.h"
#include "vulkanIndexBuffer.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include "vulkanMaterial.h"
#include "vulkanMesh.h"
#include "vulkanPipeline.h"
#include "vulkanPipelineStage.h"
#include "vulkanPoolManager.h"
#include "vulkanPostRenderCompute.h"
#include "vulkanPreRenderCompute.h"
#include "vulkanPresentRenderPass.h"
#include "vulkanRenderPassManager.h"
#include "vulkanRenderTexture2d.h"
#include "vulkanSampler.h"
#include "vulkanSampleTexture2d.h"
#include "vulkanSampleTextureCube.h"
#include "vulkanShaderProperties.h"
#include "vulkanShadowPushConstant.h"
#include "vulkanShadowRenderPass.h"
#include "vulkanStorageBuffer.h"
#include "vulkanStorageSampleTexture2d.h"
#include "vulkanStorageTexture2d.h"
#include "vulkanSurface.h"
#include "vulkanSwapchain.h"
#include "vulkanVertexBuffer.h"
#include <assert.h>
#include <string>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	Renderer::Renderer(const emberCommon::RendererCreateInfo& createInfo, emberBackendInterface::IWindow* pIWindow)
	{
		m_pIWindow = pIWindow;
		Context::Init(createInfo, pIWindow, this);

		m_time = 0.0f;
		m_deltaTime = 0.0f;
		m_rebuildSwapchain = false;
		std::filesystem::path shadersSrcDirectory = (std::filesystem::path(ENGINE_SHADERS_DIR) / "shaders" / "src").make_preferred();

		// Command pools (one per frameInFlight * renderStage):
		m_commandPools.reserve(Context::GetFramesInFlight() * (int)RenderStage::stageCount);
		for (int i = 0; i < Context::GetFramesInFlight() * (int)RenderStage::stageCount; i++)
			m_commandPools.emplace_back(emberTaskSystem::TaskSystem::GetCoreCount(), Context::GetLogicalDevice()->GetGraphicsQueue());

		m_pEngineSet = std::make_unique<EngineSet>(createInfo.maxDirectionalLights, createInfo.maxPositionalLights);

		// Shadow/Light system:
		m_depthBiasConstantFactor = 0.0f;
		m_depthBiasClamp = 0.0f;
		m_depthBiasSlopeFactor = 1.0f;
		m_directionalLightsCount = 0;
		m_positionalLightsCount = 0;
		m_maxDirectionalLights = createInfo.maxDirectionalLights;
		m_maxPositionalLights = createInfo.maxPositionalLights;
		m_shadowMapResolution = createInfo.shadowMapResolution;
		m_directionalLights.resize(m_maxDirectionalLights);
		m_positionalLights.resize(m_maxPositionalLights);
		m_lightWorldToClipMatrizes.resize(m_maxDirectionalLights + m_maxPositionalLights);
		m_pShadowMaterial = std::make_unique<Material>(m_shadowMapResolution);
		m_shadowPipelineLayout = m_pShadowMaterial->GetPipeline()->GetVkPipelineLayout();

		m_pendingMeshUpdates.resize(Context::GetFramesInFlight()); // prepare one pending mesh update vector per frame in flight.
		m_pGammaCorrectionComputeShader = std::make_unique<ComputeShader>("gammaCorrectionComputeShader", shadersSrcDirectory / "compute" / "gammaCorrection.comp.spv");

		// Debug naming:
		for (int renderStage = 0; renderStage < (int)RenderStage::stageCount; renderStage++)
			for (int frameIndex = 0; frameIndex < Context::GetFramesInFlight(); frameIndex++)
			{
				std::string name = renderStageNames[renderStage];
				name += "_frame" + std::to_string(frameIndex);
				NAME_VK_COMMAND_POOL(GetCommandPool(frameIndex, renderStage).GetPrimaryVkCommandPool(), "CommandPoolPrimary_" + name);
                NAME_VK_COMMAND_BUFFER(GetCommandPool(frameIndex, renderStage).GetPrimaryVkCommandBuffer(), "CommandBufferPrimary_" + name);
				for (int threadIndex = 0; threadIndex < emberTaskSystem::TaskSystem::GetCoreCount(); threadIndex++)
                {
                    NAME_VK_COMMAND_POOL(GetCommandPool(frameIndex, renderStage).GetSecondaryVkCommandPool(threadIndex), "CommandPoolSecondary" + std::to_string(threadIndex) + "_" + name);
                    NAME_VK_COMMAND_BUFFER(GetCommandPool(frameIndex, renderStage).GetSecondaryVkCommandBuffer(threadIndex), "CommandBufferSecondary_" + std::to_string(threadIndex) + "_" + name);
                }
			}

		// Synchronization objects:
		CreateFences();
		CreateSemaphores();
	}
	Renderer::~Renderer()
	{
		Context::WaitDeviceIdle();
		DestroySemaphores();
		DestroyFences();
		m_commandPools.clear();
		m_pShadowMaterial.reset();
		m_pGammaCorrectionComputeShader.reset();
		Context::Clear();
	}



	// Movable:
	Renderer::Renderer(Renderer&& other) noexcept = default;
	Renderer& Renderer::operator=(Renderer&& other) noexcept = default;



	// Main render call:
	void Renderer::RenderFrame(float time, float deltaTime)
	{
		m_time = time;
		m_deltaTime = deltaTime;

		// Resize Swapchain if needed:
		Int2 windowSize = m_pIWindow->GetSize();
		Uint2 surfaceExtend = Context::GetSurface()->GetCurrentExtent();
		if (m_rebuildSwapchain || windowSize.x != surfaceExtend.x || windowSize.y != surfaceExtend.y)
		{
			m_rebuildSwapchain = false;
			Context::ResetFrameIndex();
			RebuildSwapchain();
		}

		// Wait for previous frame:
		WaitForFrameFence();
		ResetCommandPools();

		// Cancel current frame on failed acquisition (e.g. window resize):
		if (!AcquireImage())
			return;

		// We use linear color space throughout entire render process. So apply gamma correction in post processing as last step:
		m_pCompute->GetPostRenderCompute()->RecordComputeShader(m_pGammaCorrectionComputeShader.get());
		SortDrawCallPointers();
		UpdateShaderData();
		
		// Record and submit current frame commands:
		{
			PROFILE_SCOPE("Record");
			DEBUG_LOG_CRITICAL("Recording frame {}", Context::GetFrameIndex());

			RecordResourceUpdateCommands();
			SubmitResourceUpdateCommands();

			RecordPreRenderComputeCommands();
			SubmitPreRenderComputeCommands();

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
		m_pCompute->GetPreRenderCompute()->ResetComputeCalls();
		ResetLights();
		ResetDrawCalls();
		m_pCompute->GetPostRenderCompute()->ResetComputeCalls();

		// Cancel current frame on failed presentation (e.g. window resize):
		if (!PresentImage())
			return;

		Context::UpdateFrameIndex();
	}



	// Add lightsources:
	void Renderer::AddDirectionalLight(const Float3& direction, float intensity, const Float3& color, emberCommon::ShadowType shadowType, const Float4x4& worldToClipMatrix)
	{
		if (m_directionalLightsCount == m_maxDirectionalLights)
			return;

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
			return;

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
	void Renderer::DrawMesh(emberBackendInterface::IMesh* pMesh, emberBackendInterface::IMaterial* pMaterial, emberBackendInterface::IShaderProperties* pIShaderProperties, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows)
	{// for static draw calls.
		if (!pMesh)
		{
			LOG_ERROR("vulkanRendererBackend::Renderer::DrawMesh(...) failed. pMesh is nullptr.");
			return;
		}
		if (!pMaterial)
		{
			LOG_ERROR("vulkanRendererBackend::Renderer::DrawMesh(...) failed. pMaterial is nullptr.");
			return;
		}
		if (!pIShaderProperties)
		{
			LOG_ERROR("vulkanRendererBackend::Renderer::DrawMesh(...) failed. pIShaderProperties is nullptr.");
			return;
		}

		// Setup draw call:
		ShaderProperties* pShadowShaderProperties = PoolManager::CheckOutShaderProperties((Shader*)m_pShadowMaterial.get());
		DrawCall drawCall = { localToWorldMatrix, receiveShadows, castShadows, static_cast<Material*>(pMaterial), static_cast<ShaderProperties*>(pIShaderProperties), pShadowShaderProperties, static_cast<Mesh*>(pMesh), 0 };
		m_staticDrawCalls.push_back(drawCall);
	}
	emberBackendInterface::IShaderProperties* Renderer::DrawMesh(emberBackendInterface::IMesh* pMesh, emberBackendInterface::IMaterial* pMaterial, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows)
	{// for dynamic draw calls.
		if (!pMesh)
		{
			LOG_ERROR("vulkanRendererBackend::Renderer::DrawMesh(...) failed. pMesh is nullptr.");
			return nullptr;
		}
		if (!pMaterial)
		{
			LOG_ERROR("vulkanRendererBackend::Renderer::DrawMesh(...) failed. pMaterial is nullptr.");
			return nullptr;
		}

		// Setup draw call:
		ShaderProperties* pShaderProperties = PoolManager::CheckOutShaderProperties(static_cast<Shader*>(static_cast<Material*>(pMaterial)));
		ShaderProperties* pShadowShaderProperties = PoolManager::CheckOutShaderProperties((Shader*)m_pShadowMaterial.get());
		DrawCall drawCall = { localToWorldMatrix, receiveShadows, castShadows, static_cast<Material*>(pMaterial), pShaderProperties, pShadowShaderProperties, static_cast<Mesh*>(pMesh), 0 };
		m_dynamicDrawCalls.push_back(drawCall);
		return pShaderProperties;
	}



	// Draw instanced:
	void Renderer::DrawInstanced(uint32_t instanceCount, emberBackendInterface::IBuffer* pInstanceBuffer, emberBackendInterface::IMesh* pMesh, emberBackendInterface::IMaterial* pMaterial, emberBackendInterface::IShaderProperties* pShaderProperties, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows)
	{
		if (!pInstanceBuffer)
		{
			LOG_ERROR("vulkanRendererBackend::Renderer::DrawInstanced(...) failed. pInstanceBuffer is nullptr.");
			return;
		}
		if (!pMesh)
		{
			LOG_ERROR("vulkanRendererBackend::Renderer::DrawInstanced(...) failed. pMesh is nullptr.");
			return;
		}
		if (!pMaterial)
		{
			LOG_ERROR("vulkanRendererBackend::Renderer::DrawInstanced(...) failed. pMaterial is nullptr.");
			return;
		}

		// Setup draw call:
		pShaderProperties->SetBuffer("instanceBuffer", pInstanceBuffer);
		ShaderProperties* pShadowShaderProperties = PoolManager::CheckOutShaderProperties((Shader*)m_pShadowMaterial.get());
		pShadowShaderProperties->SetBuffer("instanceBuffer", pInstanceBuffer);
		DrawCall drawCall = { localToWorldMatrix, receiveShadows, castShadows, static_cast<Material*>(pMaterial), static_cast<ShaderProperties*>(pShaderProperties), pShadowShaderProperties, static_cast<Mesh*>(pMesh), instanceCount };
		m_staticDrawCalls.push_back(drawCall);
	}
	emberBackendInterface::IShaderProperties* Renderer::DrawInstanced(uint32_t instanceCount, emberBackendInterface::IBuffer* pInstanceBuffer, emberBackendInterface::IMesh* pMesh, emberBackendInterface::IMaterial* pMaterial, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows)
	{
		ShaderProperties* pShaderProperties = PoolManager::CheckOutShaderProperties(static_cast<Shader*>(static_cast<Material*>(pMaterial)));
		DrawInstanced(instanceCount, pInstanceBuffer, pMesh, pMaterial, pShaderProperties, localToWorldMatrix, receiveShadows, castShadows);
		return pShaderProperties;
	}



	// Getters:
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
		RenderTexture2d* pRenderTexture = RenderPassManager::GetForwardRenderPass()->GetRenderTexture();
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



	// Functionallity forwarding:
	void Renderer::CollectGarbage()
	{
		GarbageCollector::CollectGarbage();
	}
	void Renderer::WaitDeviceIdle()
	{
		Context::WaitDeviceIdle();
	}



	// Gpu resource factories:
	emberBackendInterface::IBuffer* Renderer::CreateBuffer(uint32_t count, uint32_t elementSize, const std::string& name, emberCommon::BufferUsage usage)
	{
		emberBackendInterface::IBuffer* pIBuffer = nullptr;
		switch (usage)
		{
		case emberCommon::BufferUsage::index:
			pIBuffer = new IndexBuffer(count, elementSize, name);
			break;
		case emberCommon::BufferUsage::storage:
			pIBuffer = new StorageBuffer(count, elementSize, name);
			break;
		case emberCommon::BufferUsage::vertex:
			pIBuffer = new VertexBuffer(count, elementSize, name);
			break;
		default:
			throw std::runtime_error("vulkanRendererBackend::Renderer::CreateBuffer: Unknown invalid BufferUsage type: " + (std::string)emberCommon::BufferUsageNames[(int)usage]);
		}
		return pIBuffer;
	}
	//emberBackendInterface::ITexture* Renderer::CreateTexture1d(const std::string& name, int width, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage)
	//{
	//
	//}
	emberBackendInterface::ITexture* Renderer::CreateTexture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data)
	{
		Format vulkanFormat = TextureFormatCommonToVulkan(format);
		emberBackendInterface::ITexture* pITexture = nullptr;
		switch (usage)
		{
		case emberCommon::TextureUsage::sample:
			pITexture = new SampleTexture2d(name, vulkanFormat, width, height, data);
			break;
		case emberCommon::TextureUsage::storage:
			pITexture = new StorageTexture2d(name, vulkanFormat, width, height, data);
			break;
		case emberCommon::TextureUsage::storageSample:
			pITexture = new StorageSampleTexture2d(name, vulkanFormat, width, height, data);
			break;
		case emberCommon::TextureUsage::renderTarget:
			pITexture = new RenderTexture2d(name, vulkanFormat, width, height);
			break;
		default:
			throw std::runtime_error("vulkanRendererBackend::Renderer::CreateTexture2d: invalid TextureUsage type: " + (std::string)emberCommon::TextureUsageNames[(int)usage]);
		}
		return pITexture;
	}
	//emberBackendInterface::ITexture* Renderer::CreateTexture3d(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage)
	//{
	//
	//}
	emberBackendInterface::ITexture* Renderer::CreateTextureCube(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data)
	{
		Format vulkanFormat = TextureFormatCommonToVulkan(format);
		emberBackendInterface::ITexture* pITexture = nullptr;
		switch (usage)
		{
		case emberCommon::TextureUsage::sample:
			pITexture = new SampleTextureCube(name, vulkanFormat, width, height, data);
			break;
		default:
			throw std::runtime_error("vulkanRendererBackend::Renderer::CreateTextureCube: invalid TextureUsage type: " + (std::string)emberCommon::TextureUsageNames[(int)usage]);
		}
		return pITexture;
	}
	emberBackendInterface::IComputeShader* Renderer::CreateComputeShader(const std::string& name, const std::filesystem::path& computeSpv)
	{
		return new ComputeShader(name, computeSpv);
	}
	emberBackendInterface::IMaterial* Renderer::CreateMaterial(emberCommon::MaterialType type, const std::string& name, uint32_t renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		return new Material(type, name, renderQueue, vertexSpv, fragmentSpv);
	}
	emberBackendInterface::IMesh* Renderer::CreateMesh(const std::string& name)
	{
		return new Mesh(name);
	}
	emberBackendInterface::IShaderProperties* Renderer::CreateShaderProperties(emberBackendInterface::IComputeShader* pIComputeShader)
	{
		ComputeShader* pComputeShader = static_cast<ComputeShader*>(pIComputeShader);
		Shader* pShader = static_cast<Shader*>(pComputeShader);
		return new ShaderProperties(pShader);
	}
	emberBackendInterface::IShaderProperties* Renderer::CreateShaderProperties(emberBackendInterface::IMaterial* pIMaterial)
	{
		Material* pMaterial = static_cast<Material*>(pIMaterial);
		Shader* pShader = static_cast<Shader*>(pMaterial);
		return new ShaderProperties(pShader);
	}



	// Vulkan handle passthrough for API coupling:
	void* Renderer::GetVkInstance()
	{
		return static_cast<void*>(Context::GetVkInstance());
	}
	void* Renderer::GetVkPhysicalDevice()
	{
		return static_cast<void*>(Context::GetVkPhysicalDevice());
	}
	void* Renderer::GetVkDevice()
	{
		return static_cast<void*>(Context::GetVkDevice());
	}
	void* Renderer::GetPresentVkRenderPass()
	{
		return static_cast<void*>(RenderPassManager::GetPresentRenderPass()->GetVkRenderPass());
	}
	void* Renderer::GetVkDescriptorPool()
	{
		return static_cast<void*>(Context::GetVkDescriptorPool());
	}
	void* Renderer::GetGraphicsVkQueue()
	{
		return static_cast<void*>(Context::GetLogicalDevice()->GetGraphicsQueue().queue);
	}
	void* Renderer::GetColorSampler()
	{
		return static_cast<void*>(DefaultGpuResources::GetColorSampler()->GetVkSampler());
	}
	uint32_t Renderer::GetGraphicsVkQueueFamilyIndex()
	{
		return Context::GetLogicalDevice()->GetGraphicsQueue().familyIndex;
	}
	uint32_t Renderer::GetSwapchainImageCount()
	{
		return Context::GetSwapchain()->GetImageCount();
	}
	uint32_t Renderer::GetFramesInFlight()
	{
		return Context::GetFramesInFlight();
	}



	// Backend only:
	void Renderer::QueueMeshForUpdate(vulkanRendererBackend::Mesh* pMesh)
	{
		// Prevent double-adding:
		for (std::vector<Mesh*>& meshUpdates : m_pendingMeshUpdates)
		{
			if (std::find(meshUpdatesbegin(), meshUpdates.end(), pMesh) == meshUpdates.end())
				meshUpdates.push_back(pMesh);
		}
	}



	// Private methods:
	// Reset render state:
	void Renderer::ResetLights()
	{
		m_directionalLightsCount = 0;
		m_positionalLightsCount = 0;
	}
	void Renderer::ResetDrawCalls()
	{
		// Return all pShaderProperties/pShadowShaderProperties of dynamic draw calls back to the corresponding pool:
		for (DrawCall& drawCall : m_dynamicDrawCalls)
		{
			PoolManager::ReturnShaderProperties((Shader*)drawCall.pMaterial, drawCall.pShaderProperties);
			PoolManager::ReturnShaderProperties((Shader*)m_pShadowMaterial.get(), drawCall.pShadowShaderProperties);
		}

		// Return all pShadowShaderProperties of static draw calls back to the pool:
		for (DrawCall& drawCall : m_staticDrawCalls)
		{
			PoolManager::ReturnShaderProperties((Shader*)m_pShadowMaterial.get(), drawCall.pShadowShaderProperties);
		}

		// Clear all draw calls for next frame:
		m_staticDrawCalls.clear();
		m_dynamicDrawCalls.clear();
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
		PROFILE_FUNCTION();

		// Signal acquireSemaphore when done:
		VkResult result = vkAcquireNextImageKHR(Context::GetVkDevice(), Context::GetVkSwapchainKHR(), UINT64_MAX, m_acquireSemaphores[Context::GetFrameIndex()], VK_NULL_HANDLE, &m_imageIndex);

		// Resize if needed:
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_pIWindow->GetIsResized())
		{
			m_pIWindow->ResetWindowResized();
			m_rebuildSwapchain = true;
			return false;
		}
		else
		{
			VKA(result);
			return true;
		}
	}
	void Renderer::SortDrawCallPointers()
	{
		// Populate sorted draw call pointers vector:
		m_sortedDrawCallPointers.clear();
		m_sortedDrawCallPointers.reserve(m_staticDrawCalls.size() + m_dynamicDrawCalls.size());
		for (auto& drawCall : m_staticDrawCalls)
			m_sortedDrawCallPointers.push_back(&drawCall);
		for (auto& drawCall : m_dynamicDrawCalls)
			m_sortedDrawCallPointers.push_back(&drawCall);

		// Sort by renderQueue first, then interleaved before separate:
		std::sort(m_sortedDrawCallPointers.begin(), m_sortedDrawCallPointers.end(), [](DrawCall* a, DrawCall* b)
		{
			// RenderQueue:
			int renderQueueA = static_cast<int>(a->pMaterial->GetRenderQueue());
			int renderQueueB = static_cast<int>(b->pMaterial->GetRenderQueue());
			if (renderQueueA != renderQueueB)
				return renderQueueA < renderQueueB;

			// Interleaved(0) before separate(1):
			auto layoutA = a->pMesh->GetVertexMemoryLayout();
			auto layoutB = b->pMesh->GetVertexMemoryLayout();
			if (layoutA != layoutB)
				return layoutA < layoutB;

			// Tie-breaker by pointer:
			return a < b;
		});
	}
	void Renderer::UpdateShaderData()
	{
		// Pre render compute:
		for (ComputeCall* computeCall : m_pCompute->GetPreRenderCompute()->GetComputeCallPointers())
			if (computeCall->pComputeShader)
				computeCall->pShaderProperties->UpdateShaderData();

		// Light matrizes:
		int shadowLightCount = 0;
		for (uint32_t i = 0; i < m_directionalLightsCount; i++)
			m_lightWorldToClipMatrizes[shadowLightCount++] = m_directionalLights[i].worldToClipMatrix;
		for (uint32_t i = 0; i < m_positionalLightsCount; i++)
			m_lightWorldToClipMatrizes[shadowLightCount++] = m_positionalLights[i].worldToClipMatrix;

		// Forward calls:
		for (DrawCall* drawCall : m_sortedDrawCallPointers)
		{
			drawCall->pShadowShaderProperties->UpdateShaderData();
			drawCall->SetRenderMatrizes(m_activeCamera.viewMatrix, m_activeCamera.projectionMatrix);
			drawCall->SetLightData(m_directionalLights, m_positionalLights);
			drawCall->pShaderProperties->UpdateShaderData();
		}

		// Post render compute:
		for (ComputeCall* computeCall : m_pCompute->GetPostRenderCompute()->GetComputeCallPointers())
		{
			if (computeCall->callIndex % 2 == 0)
			{
				computeCall->pShaderProperties->SetTexture("inputImage", RenderPassManager::GetForwardRenderPass()->GetRenderTexture());
				computeCall->pShaderProperties->SetTexture("outputImage", RenderPassManager::GetForwardRenderPass()->GetSecondaryRenderTexture());
			}
			else
			{
				computeCall->pShaderProperties->SetTexture("inputImage", RenderPassManager::GetForwardRenderPass()->GetSecondaryRenderTexture());
				computeCall->pShaderProperties->SetTexture("outputImage", RenderPassManager::GetForwardRenderPass()->GetRenderTexture());
			}
			computeCall->pShaderProperties->UpdateShaderData();
		}

		// Present call:
		ShaderProperties* pPresentShaderProperties = DefaultGpuResources::GetDefaultPresentShaderProperties();
		pPresentShaderProperties->SetTexture("renderTexture", RenderPassManager::GetForwardRenderPass()->GetRenderTexture());
		pPresentShaderProperties->UpdateShaderData();
	}



	// Wait for fence:
	void Renderer::WaitForFrameFence()
	{
		PROFILE_FUNCTION();
		VKA(vkWaitForFences(Context::GetVkDevice(), 1, &m_frameFences[Context::GetFrameIndex()], VK_TRUE, UINT64_MAX));
		VKA(vkResetFences(Context::GetVkDevice(), 1, &m_frameFences[Context::GetFrameIndex()]));
	}
	void Renderer::ResetCommandPools()
	{
		GetCommandPool(Context::GetFrameIndex(), RenderStage::resourceUpdate).ResetPools();
		GetCommandPool(Context::GetFrameIndex(), RenderStage::preRenderCompute).ResetPools();
		GetCommandPool(Context::GetFrameIndex(), RenderStage::shadow).ResetPools();
		GetCommandPool(Context::GetFrameIndex(), RenderStage::forward).ResetPools();
		GetCommandPool(Context::GetFrameIndex(), RenderStage::postRenderCompute).ResetPools();
		GetCommandPool(Context::GetFrameIndex(), RenderStage::present).ResetPools();
	}



	// Record commands:
	void Renderer::RecordResourceUpdateCommands()
	{
		PROFILE_FUNCTION();

		// Prepare meshes to update:
		uint32_t frameIndex = Context::GetFrameIndex();
		std::vector<Mesh*>& meshUpdates = m_pendingMeshUpdates[frameIndex];
		if (meshUpdates.empty())
			return;

		// Prepare command recording:
		CommandPool& commandPool = GetCommandPool(frameIndex, RenderStage::resourceUpdate);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		// Record resource update commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			for (Mesh* mesh : meshUpdates)
				mesh->RecordUpdateCommand(commandBuffer, frameIndex);
		}
		VKA(vkEndCommandBuffer(commandBuffer));

		// Clear only mesh updates of current frame:
		meshUpdates.clear();
	}
	void Renderer::RecordPreRenderComputeCommands()
	{
		PROFILE_FUNCTION();

		// Prepare command recording:
		CommandPool& commandPool = GetCommandPool(Context::GetFrameIndex(), RenderStage::preRenderCompute);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		// Record pre render compute commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			ComputeShader* pComputeShader = nullptr;
			ComputeShader* pPreviousComputeShader = nullptr;
			VkPipeline pipeline = VK_NULL_HANDLE;
			VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
			ComputePushConstant pushConstant(Uint3::one, m_time, m_deltaTime);

			for (ComputeCall* computeCall : m_pCompute->GetPreRenderCompute()->GetComputeCallPointers())
			{
				// Compute call is a barrier:
				if (computeCall->pComputeShader == nullptr)
				{
					VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
					memoryBarrier.srcStageMask = PipelineStages::computeShader;
					memoryBarrier.dstStageMask = PipelineStages::computeShader;
					memoryBarrier.srcAccessMask = computeCall->srcAccessMask;
					memoryBarrier.dstAccessMask = computeCall->dstAccessMask;

					VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
					dependencyInfo.memoryBarrierCount = 1;
					dependencyInfo.pMemoryBarriers = &memoryBarrier;

					vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
					DEBUG_LOG_TRACE("Pre Render Compute Barrier, callIndex = {}", computeCall->callIndex);
				}
				// Compute call is a dispatch:
				else
				{
					// Change pipeline if compute shader has changed:
					pComputeShader = computeCall->pComputeShader;
					if (pPreviousComputeShader != pComputeShader)
					{
						pPreviousComputeShader = pComputeShader;
						pipeline = pComputeShader->GetPipeline()->GetVkPipeline();
						pipelineLayout = pComputeShader->GetPipeline()->GetVkPipelineLayout();
						pushConstant.threadCount = computeCall->threadCount;
						vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
						pushConstant.threadCount = computeCall->threadCount;
						vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstant), &pushConstant);
					}

					// Same compute shader but different thread Count => update push constants:
					if (pushConstant.threadCount != computeCall->threadCount)
					{
						pushConstant.threadCount = computeCall->threadCount;
						vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstant), &pushConstant);
					}

					Uint3 blockSize = pComputeShader->GetBlockSize();
					uint32_t groupCountX = (computeCall->threadCount[0] + blockSize[0] - 1) / blockSize[0];
					uint32_t groupCountY = (computeCall->threadCount[1] + blockSize[1] - 1) / blockSize[1];
					uint32_t groupCountZ = (computeCall->threadCount[2] + blockSize[2] - 1) / blockSize[2];

					// Ember::ToDo: bind all descriptor sets 0-4
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &computeCall->pShaderProperties->GetDescriptorSet(Context::GetFrameIndex()), 0, nullptr);
					vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
					DEBUG_LOG_TRACE("Pre Render Compute Shader {}, callIndex = {}", computeCall->pComputeShader->GetName(), computeCall->callIndex);
				}
			}

			// Release memory from pre render compute shaders to vertex shaders:
			{
				VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
				memoryBarrier.srcStageMask = PipelineStages::computeShader;
				memoryBarrier.dstStageMask = PipelineStages::vertexShader;
				memoryBarrier.srcAccessMask = AccessMasks::ComputeShader::shaderWrite;
				memoryBarrier.dstAccessMask = AccessMasks::VertexShader::shaderRead;

				VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
				dependencyInfo.memoryBarrierCount = 1;
				dependencyInfo.pMemoryBarriers = &memoryBarrier;

				vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
				DEBUG_LOG_TRACE("Memory Barrier: pre compute to vertex");
			}
		}
		VKA(vkEndCommandBuffer(commandBuffer));
	}
	void Renderer::RecordShadowCommands()
	{
		PROFILE_FUNCTION();

		// Prepare command recording:
		CommandPool& commandPool = GetCommandPool(Context::GetFrameIndex(), RenderStage::shadow);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		// Record shadow commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			// Render pass info:
			VkClearValue clearValues = {};
			clearValues.depthStencil = { 1.0f, 0 };
			VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
			renderPassBeginInfo.renderPass = RenderPassManager::GetShadowRenderPass()->GetVkRenderPass();
			renderPassBeginInfo.framebuffer = RenderPassManager::GetShadowRenderPass()->GetFramebuffer(0);
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent = VkExtent2D{ m_shadowMapResolution, m_shadowMapResolution };
			renderPassBeginInfo.clearValueCount = 1;
			renderPassBeginInfo.pClearValues = &clearValues;

			// Begin render pass:
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			{
				VkPipeline pipeline = VK_NULL_HANDLE;

				// Lights:
				const uint32_t shadowLightCount = m_directionalLightsCount + m_positionalLightsCount;
				if (shadowLightCount > 0)
				{
					// Dynamic state: depth bias:
					vkCmdSetDepthBias(commandBuffer, m_depthBiasConstantFactor, m_depthBiasClamp, m_depthBiasSlopeFactor);

					// Draw calls:
					for (DrawCall* drawCall : m_sortedDrawCallPointers)
					{
						if (drawCall->castShadows == false)
							continue;

						// Pipeline swap:
						if (pipeline != m_pShadowMaterial->GetPipeline(drawCall->pMesh)->GetVkPipeline())
						{
							pipeline = m_pShadowMaterial->GetPipeline()->GetVkPipeline();
							vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
						}

						vkCmdBindVertexBuffers(commandBuffer, 0, drawCall->pMesh->GetVertexBindingCount(), drawCall->pMesh->GetVkBuffers(), drawCall->pMesh->GetOffsets());
						vkCmdBindIndexBuffer(commandBuffer, drawCall->pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, drawCall->pMesh->GetVkIndexType());

						// Ember::ToDo: bind all descriptor sets 0-4
						vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_shadowPipelineLayout, 0, 1, &drawCall->pShadowShaderProperties->GetDescriptorSet(Context::GetFrameIndex()), 0, nullptr);

						for (uint32_t shadowMapIndex = 0; shadowMapIndex < shadowLightCount; shadowMapIndex++)
						{
							ShadowPushConstant pushConstant(drawCall->instanceCount, shadowMapIndex, drawCall->localToWorldMatrix, m_lightWorldToClipMatrizes[shadowMapIndex]);
							vkCmdPushConstants(commandBuffer, m_shadowPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ShadowPushConstant), &pushConstant);
							vkCmdDrawIndexed(commandBuffer, drawCall->pMesh->GetIndexCount(), std::max(drawCall->instanceCount, (uint32_t)1), 0, 0, 0);
							DEBUG_LOG_INFO("Light {}, mesh = {}", shadowMapIndex, drawCall->pMesh->GetName());
						}
					}
				}
			}
			vkCmdEndRenderPass(commandBuffer);
		}
		VKA(vkEndCommandBuffer(commandBuffer));
	}
	void Renderer::RecordForwardCommands()
	{
		PROFILE_FUNCTION();

		VkDescriptorSet boundSets[5] =
		{
			VK_NULL_HANDLE,
			VK_NULL_HANDLE,
			VK_NULL_HANDLE,
			VK_NULL_HANDLE,
			VK_NULL_HANDLE
		};
		// Use this array for:
		if (boundSets[setIndex] != newSet)
		{
			vkCmdBindDescriptorSets(...);
			boundSets[setIndex] = newSet;
		}


		// Any compatible pipeline layout, e.g. 0th draw calls:
		VkPipelineLayout layout = drawCall->pMaterial->GetPipeline()->GetVkPipelineLayout(); 

		// Bind these only once after vkCmdBeginRenderPass
		{ // Engine set (0), once per command buffer
			VkDescriptorSet ds = DefaultGpuResources::GetDefaultShaderProperties()
				->GetDescriptorSet(ENGINE_SET_INDEX, Context::GetFrameIndex())
				.GetVkDescriptorSet();

			vkCmdBindDescriptorSets(
				commandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				layout,
				ENGINE_SET_INDEX,
				1, &ds,
				0, nullptr
			);

			boundSets[ENGINE_SET_INDEX] = ds;
		}
		{ // Scene set (1)
			VkDescriptorSet ds = DefaultGpuResources::GetDefaultShaderProperties()
				->GetDescriptorSet(SCENE_SET_INDEX, Context::GetFrameIndex())
				.GetVkDescriptorSet();

			vkCmdBindDescriptorSets(... set = 1 ...);
			boundSets[SCENE_SET_INDEX] = ds;
		}
		{// Frame set (2)
			VkDescriptorSet ds = DefaultGpuResources::GetDefaultShaderProperties()
				->GetDescriptorSet(FRAME_SET_INDEX, Context::GetFrameIndex())
				.GetVkDescriptorSet();

			vkCmdBindDescriptorSets(... set = 2 ...);
			boundSets[FRAME_SET_INDEX] = ds;
		}

		// In draw call Loop:
		{// Material set (3)
			VkDescriptorSet materialSet =
				drawCall->pMaterial->GetShaderProperties()
				->GetDescriptorSet(MATERIAL_SET_INDEX, Context::GetFrameIndex())
				.GetVkDescriptorSet();

			if (boundSets[MATERIAL_SET_INDEX] != materialSet)
			{
				vkCmdBindDescriptorSets(
					commandBuffer,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					layout,
					MATERIAL_SET_INDEX,
					1, &materialSet,
					0, nullptr
				);
				boundSets[MATERIAL_SET_INDEX] = materialSet;
			}
		}
		{// Draw set (4)
			VkDescriptorSet drawSet =
				drawCall->pShaderProperties
				->GetDescriptorSet(DRAW_SET_INDEX, Context::GetFrameIndex())
				.GetVkDescriptorSet();

			// always changes:
			vkCmdBindDescriptorSets(
				commandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				layout,
				DRAW_SET_INDEX,
				1, &drawSet,
				0, nullptr
			);

			boundSets[DRAW_SET_INDEX] = drawSet;
		}




		// Prepare command recording:
		CommandPool& commandPool = GetCommandPool(Context::GetFrameIndex(), RenderStage::forward);
		commandPool.ResetPools();
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		// Record forward commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			// Viewport and scissor:
			VkViewport viewport = {};
			viewport.width = RenderPassManager::GetForwardRenderPass()->GetRenderTexture()->GetWidth();
			viewport.height = RenderPassManager::GetForwardRenderPass()->GetRenderTexture()->GetHeight();
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
			renderPassBeginInfo.renderPass = RenderPassManager::GetForwardRenderPass()->GetVkRenderPass();
			renderPassBeginInfo.framebuffer = RenderPassManager::GetForwardRenderPass()->GetFramebuffer(0);
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent.width = viewport.width;
			renderPassBeginInfo.renderArea.extent.height = viewport.height;
			renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassBeginInfo.pClearValues = clearValues.data();

			// Begin render pass:
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			{
				VkPipeline pipeline = VK_NULL_HANDLE;
				DefaultPushConstant pushConstant(0, m_time, m_deltaTime, m_directionalLightsCount, m_positionalLightsCount, m_activeCamera.position);

				// Draw calls:
				for (DrawCall* drawCall : m_sortedDrawCallPointers)
				{
					PROFILE_SCOPE("DrawCall");

					// Pipeline swap:
					if (pipeline != drawCall->pMaterial->GetPipeline(drawCall->pMesh)->GetVkPipeline())
					{
						pipeline = drawCall->pMaterial->GetPipeline(drawCall->pMesh)->GetVkPipeline();
						pushConstant.instanceCount = drawCall->instanceCount;
						vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
						vkCmdPushConstants(commandBuffer, drawCall->pMaterial->GetPipeline()->GetVkPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(DefaultPushConstant), &pushConstant);
					}

					// Same pipeline but different instance Count => update push constants:
					if (pushConstant.instanceCount != drawCall->instanceCount)
					{
						pushConstant.instanceCount = drawCall->instanceCount;
						vkCmdPushConstants(commandBuffer, drawCall->pMaterial->GetPipeline()->GetVkPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(DefaultPushConstant), &pushConstant);
					}

					vkCmdBindVertexBuffers(commandBuffer, 0, drawCall->pMesh->GetVertexBindingCount(), drawCall->pMesh->GetVkBuffers(), drawCall->pMesh->GetOffsets());
					vkCmdBindIndexBuffer(commandBuffer, drawCall->pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, drawCall->pMesh->GetVkIndexType());
					// Ember::ToDo: bind all descriptor sets 0-4
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, drawCall->pMaterial->GetPipeline()->GetVkPipelineLayout(), 0, 1, &drawCall->pShaderProperties->GetDescriptorSet(Context::GetFrameIndex()), 0, nullptr);
					vkCmdDrawIndexed(commandBuffer, drawCall->pMesh->GetIndexCount(), std::max(drawCall->instanceCount, (uint32_t)1), 0, 0, 0);
					DEBUG_LOG_WARN("Forward draw call, mesh = {}, material = {}", drawCall->pMesh->GetName(), drawCall->pMaterial->GetName());
				}
			}
			vkCmdEndRenderPass(commandBuffer);

			// Release memory from vertex shaders to post render compute shaders:
			{
				VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
				memoryBarrier.srcStageMask = PipelineStages::vertexShader;
				memoryBarrier.dstStageMask = PipelineStages::computeShader;
				memoryBarrier.srcAccessMask = AccessMasks::VertexShader::shaderRead;
				memoryBarrier.dstAccessMask = AccessMasks::ComputeShader::shaderWrite;

				VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
				dependencyInfo.memoryBarrierCount = 1;
				dependencyInfo.pMemoryBarriers = &memoryBarrier;

				vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
				DEBUG_LOG_TRACE("Memory Barrier: vertex to post compute");
			}
		}
		VKA(vkEndCommandBuffer(commandBuffer));

		// Forward render pass's color resolve finalLayout is VK_IMAGE_LAYOUT_GENERAL. Reflect this in the image layout:
		RenderPassManager::GetForwardRenderPass()->GetRenderTexture()->GetVmaImage()->SetLayout(VK_IMAGE_LAYOUT_GENERAL);
	}
	void Renderer::RecordForwardCommandsParallel()
	{
		PROFILE_FUNCTION();

		// Logic for workload splitting across threads:
		int totalWorkload = (int)m_sortedDrawCallPointers.size();
		int threadIndex = emberTaskSystem::TaskSystem::GetThreadIndex();
		int coreCount = emberTaskSystem::TaskSystem::GetCoreCount();
		int baseChunkSize = totalWorkload / coreCount;
		int remainder = totalWorkload % coreCount;
		int startIndex = threadIndex * baseChunkSize + std::min(threadIndex, remainder);
		int endIndex = startIndex + baseChunkSize + (threadIndex < remainder ? 1 : 0);

		// Prepare command recording:
		CommandPool& commandPool = GetCommandPool(Context::GetFrameIndex(), RenderStage::forward);
		VkCommandBuffer& secondaryCommandBuffer = commandPool.GetSecondaryVkCommandBuffer(threadIndex);

		VkCommandBufferInheritanceInfo inheritanceInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO };
		inheritanceInfo.renderPass = RenderPassManager::GetForwardRenderPass()->GetVkRenderPass();
		inheritanceInfo.framebuffer = RenderPassManager::GetForwardRenderPass()->GetFramebuffer(0);
		inheritanceInfo.subpass = 0;

		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		beginInfo.pInheritanceInfo = &inheritanceInfo;

		// Record forward commands:
		VKA(vkBeginCommandBuffer(secondaryCommandBuffer, &beginInfo));
		{
			// Viewport and scissor:
			VkViewport viewport = {};
			viewport.width = RenderPassManager::GetForwardRenderPass()->GetRenderTexture()->GetWidth();
			viewport.height = RenderPassManager::GetForwardRenderPass()->GetRenderTexture()->GetHeight();
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			VkRect2D scissor = {};
			scissor.extent.width = viewport.width;
			scissor.extent.height = viewport.height;
			vkCmdSetViewport(secondaryCommandBuffer, 0, 1, &viewport);
			vkCmdSetScissor(secondaryCommandBuffer, 0, 1, &scissor);

			// Record commands: (no begin renderpass for secondary command buffers)
			{
				VkPipeline pipeline = VK_NULL_HANDLE;
				DefaultPushConstant pushConstant(0, m_time, m_deltaTime, m_directionalLightsCount, m_positionalLightsCount, m_activeCamera.position);

				// Draw calls:
				for (int i = startIndex; i < endIndex; i++)
				{
					DrawCall* drawCall = (m_sortedDrawCallPointers)[i];

					// Pipeline swap:
					if (pipeline != drawCall->pMaterial->GetPipeline(drawCall->pMesh)->GetVkPipeline())
					{
						pipeline = drawCall->pMaterial->GetPipeline(drawCall->pMesh)->GetVkPipeline();
						pushConstant.instanceCount = drawCall->instanceCount;
						vkCmdBindPipeline(secondaryCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
						vkCmdPushConstants(secondaryCommandBuffer, drawCall->pMaterial->GetPipeline()->GetVkPipelineLayout(); , VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(DefaultPushConstant), &pushConstant);
					}

					// Same pipeline but different instance Count => update push constants:
					if (pushConstant.instanceCount != drawCall->instanceCount)
					{
						pushConstant.instanceCount = drawCall->instanceCount;
						vkCmdPushConstants(secondaryCommandBuffer, drawCall->pMaterial->GetPipeline()->GetVkPipelineLayout(); , VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(DefaultPushConstant), &pushConstant);
					}

					vkCmdBindVertexBuffers(secondaryCommandBuffer, 0, drawCall->pMesh->GetVertexBindingCount(), drawCall->pMesh->GetVkBuffers(), drawCall->pMesh->GetOffsets());
					vkCmdBindIndexBuffer(secondaryCommandBuffer, drawCall->pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, drawCall->pMesh->GetVkIndexType());

					// Ember::ToDo: bind all descriptor sets 0-4
					vkCmdBindDescriptorSets(secondaryCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, drawCall->pMaterial->GetPipeline()->GetVkPipelineLayout(); , 0, 1, &drawCall->pShaderProperties->GetDescriptorSet(Context::GetFrameIndex()), 0, nullptr);
					vkCmdDrawIndexed(secondaryCommandBuffer, drawCall->pMesh->GetIndexCount(), std::max(drawCall->instanceCount, (uint32_t)1), 0, 0, 0);
					DEBUG_LOG_WARN("Forward draw call, mesh = {}, material = {}", drawCall->pMesh->GetName(), drawCall->pMaterial->GetName());
				}
			}
		}
		VKA(vkEndCommandBuffer(secondaryCommandBuffer));

		// Forward render pass's color resolve finalLayout is VK_IMAGE_LAYOUT_GENERAL. Reflect this in the image layout:
		RenderPassManager::GetForwardRenderPass()->GetRenderTexture()->GetVmaImage()->SetLayout(VK_IMAGE_LAYOUT_GENERAL);
	}
	void Renderer::RecordPostRenderComputeCommands()
	{
		PROFILE_FUNCTION();

		// Prepare command recording:
		CommandPool& commandPool = GetCommandPool(Context::GetFrameIndex(), RenderStage::postRenderCompute);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		// Record post render compute commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			ComputeShader* pComputeShader = nullptr;
			ComputeShader* pPreviousComputeShader = nullptr;
			VkPipeline pipeline = VK_NULL_HANDLE;
			VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
			ComputePushConstant pushConstant(Uint3::one, m_time, m_deltaTime);

			for (ComputeCall* computeCall : m_pCompute->GetPostRenderCompute()->GetComputeCallPointers())
			{
				// Change pipeline if compute shader has changed:
				pComputeShader = computeCall->pComputeShader;
				if (pPreviousComputeShader != pComputeShader)
				{
					pPreviousComputeShader = pComputeShader;
					pipeline = pComputeShader->GetPipeline()->GetVkPipeline();
					pipelineLayout = pComputeShader->GetPipeline()->GetVkPipelineLayout();
					pushConstant.threadCount = computeCall->threadCount;
					vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
					pushConstant.threadCount = computeCall->threadCount;
					vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstant), &pushConstant);
				}

				// Same compute shader but different thread Count => update push constants:
				if (pushConstant.threadCount != computeCall->threadCount)
				{
					pushConstant.threadCount = computeCall->threadCount;
					vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstant), &pushConstant);
				}

				Uint3 blockSize = pComputeShader->GetBlockSize();
				uint32_t groupCountX = (computeCall->threadCount.x + blockSize.x - 1) / blockSize.x;
				uint32_t groupCountY = (computeCall->threadCount.y + blockSize.y - 1) / blockSize.y;
				uint32_t groupCountZ = (computeCall->threadCount.z + blockSize.z - 1) / blockSize.z;

				// Ember::ToDo: bind all descriptor sets 0-4
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &computeCall->pShaderProperties->GetDescriptorSet(Context::GetFrameIndex()), 0, nullptr);
				vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
				DEBUG_LOG_ERROR("Post Render Compute Shader {}, callIndex = {}", computeCall->pComputeShader->GetName(), computeCall->callIndex);

				// All effects access the same inputImage and outputImage.
				// Thus add memory barrier between every effect:
				{
					VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
					memoryBarrier.srcStageMask = PipelineStages::computeShader;
					memoryBarrier.dstStageMask = PipelineStages::computeShader;
					memoryBarrier.srcAccessMask = AccessMasks::ComputeShader::shaderWrite;
					memoryBarrier.dstAccessMask = AccessMasks::ComputeShader::shaderRead;

					VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
					dependencyInfo.memoryBarrierCount = 1;
					dependencyInfo.pMemoryBarriers = &memoryBarrier;

					vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
					DEBUG_LOG_ERROR("Post Render Compute Barrier, callIndex = {}", computeCall->callIndex);
				}
			}

			// Transition renderTexture image to shader read only optimal:
			{
				VkImageLayout newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				PipelineStage srcStage = PipelineStages::computeShader;
				PipelineStage dstStage = PipelineStages::fragmentShader;
				VkAccessFlags2 srcAccessMask = AccessMasks::ComputeShader::shaderWrite;
				VkAccessFlags2 dstAccessMask = AccessMasks::FragmentShader::shaderRead;
				RenderPassManager::GetForwardRenderPass()->GetRenderTexture()->GetVmaImage()->TransitionLayout(commandBuffer, newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
				DEBUG_LOG_ERROR("Render Image Transition: shader write -> shader read only");
			}
		}
		VKA(vkEndCommandBuffer(commandBuffer));
	}
	void Renderer::RecordPresentCommands()
	{
		PROFILE_FUNCTION();

		// Prepare command recording:
		CommandPool& commandPool = GetCommandPool(Context::GetFrameIndex(), RenderStage::present);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		// Record present commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			Material* pMaterial = DefaultGpuResources::GetDefaultPresentMaterial();
			ShaderProperties* pPresentShaderProperties = DefaultGpuResources::GetDefaultPresentShaderProperties();
			Mesh* pMesh = DefaultGpuResources::GetDefaultRenderQuad();
			Uint2 surfaceExtend = Context::GetSurface()->GetCurrentExtent();

			// Viewport and scissor:
			VkViewport viewport = {};
			viewport.width = (float)surfaceExtend.x;
			viewport.height = (float)surfaceExtend.y;
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
			renderPassBeginInfo.renderArea.extent = VkExtent2D(surfaceExtend.x, surfaceExtend.y);


			// Begin render pass:
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			{
				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pMaterial->GetPipeline()->GetVkPipeline());

				vkCmdBindVertexBuffers(commandBuffer, 0, pMesh->GetVertexBindingCount(), pMesh->GetVkBuffers(), pMesh->GetOffsets());
				vkCmdBindIndexBuffer(secondarycommandBufferCommandBuffer, pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, pMesh->GetVkIndexType());

				// Ember::ToDo: bind all descriptor sets 0-4
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pMaterial->GetPipeline()->GetVkPipelineLayout(), 0, 1, &pPresentShaderProperties->GetDescriptorSet(Context::GetFrameIndex()), 0, nullptr);
				vkCmdDrawIndexed(commandBuffer, pMesh->GetIndexCount(), 1, 0, 0, 0);
				DEBUG_LOG_INFO("Render renderTexture into fullScreenRenderQuad, material = {}", pMaterial->GetName());
				
				m_pIGui->Render(commandBuffer);
			}
			vkCmdEndRenderPass(commandBuffer);
		}
		VKA(vkEndCommandBuffer(commandBuffer));
	}
	void Renderer::RecordImGuiPresentCommands()
	{
		PROFILE_FUNCTION();

		// Prepare command recording:
		CommandPool& commandPool = GetCommandPool(Context::GetFrameIndex(), RenderStage::present);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		// Record present commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			Uint2 surfaceExtend = Context::GetSurface()->GetCurrentExtent();

			// Render pass info:
			PresentRenderPass* presentRenderPass = RenderPassManager::GetPresentRenderPass();
			VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
			renderPassBeginInfo.renderPass = presentRenderPass->GetVkRenderPass();
			renderPassBeginInfo.framebuffer = presentRenderPass->GetFramebuffer(m_imageIndex);
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent = VkExtent2D(surfaceExtend.x, surfaceExtend.y);

			// Begin render pass:
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			{
				m_pIGui->Render(commandBuffer);
			}
			vkCmdEndRenderPass(commandBuffer);
		}
		VKA(vkEndCommandBuffer(commandBuffer));
	}



	// Submit commands:
	void Renderer::SubmitResourceUpdateCommands()
	{
		PROFILE_FUNCTION();
		CommandPool& commandPool = GetCommandPool(Context::GetFrameIndex(), RenderStage::resourceUpdate);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();

		// Wait semaphore info:
		VkSemaphoreSubmitInfo waitSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		waitSemaphoreInfo.semaphore = m_acquireSemaphores[Context::GetFrameIndex()];
		waitSemaphoreInfo.stageMask = PipelineStages::topOfPipe;

		// Command buffer info:
		VkCommandBufferSubmitInfo cmdBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
		cmdBufferInfo.commandBuffer = commandBuffer;

		// Signal semaphore info:
		VkSemaphoreSubmitInfo signalSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		signalSemaphoreInfo.semaphore = m_resourceUpdateToPreRenderComputeSemaphores[Context::GetFrameIndex()];
		signalSemaphoreInfo.stageMask = PipelineStages::transfer;

		// Submit info:
		VkSubmitInfo2 submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
		submitInfo.waitSemaphoreInfoCount = 1;
		submitInfo.pWaitSemaphoreInfos = &waitSemaphoreInfo;
		submitInfo.commandBufferInfoCount = 1;
		submitInfo.pCommandBufferInfos = &cmdBufferInfo;
		submitInfo.signalSemaphoreInfoCount = 1;
		submitInfo.pSignalSemaphoreInfos = &signalSemaphoreInfo;

		// Submit:
		VKA(vkQueueSubmit2(Context::GetLogicalDevice()->GetGraphicsQueue().queue, 1, &submitInfo, VK_NULL_HANDLE));
	}
	void Renderer::SubmitPreRenderComputeCommands()
	{
		PROFILE_FUNCTION();
		CommandPool& commandPool = GetCommandPool(Context::GetFrameIndex(), RenderStage::preRenderCompute);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();

		// Wait semaphore info:
		VkSemaphoreSubmitInfo waitSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		waitSemaphoreInfo.semaphore = m_resourceUpdateToPreRenderComputeSemaphores[Context::GetFrameIndex()];
		waitSemaphoreInfo.stageMask = PipelineStages::topOfPipe;

		// Command buffer info:
		VkCommandBufferSubmitInfo cmdBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
		cmdBufferInfo.commandBuffer = commandBuffer;

		// Signal semaphore info:
		VkSemaphoreSubmitInfo signalSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		signalSemaphoreInfo.semaphore = m_preRenderComputeToShadowSemaphores[Context::GetFrameIndex()];
		signalSemaphoreInfo.stageMask = PipelineStages::computeShader;

		// Submit info:
		VkSubmitInfo2 submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
		submitInfo.waitSemaphoreInfoCount = 1;
		submitInfo.pWaitSemaphoreInfos = &waitSemaphoreInfo;
		submitInfo.commandBufferInfoCount = 1;
		submitInfo.pCommandBufferInfos = &cmdBufferInfo;
		submitInfo.signalSemaphoreInfoCount = 1;
		submitInfo.pSignalSemaphoreInfos = &signalSemaphoreInfo;

		// Submit:
		VKA(vkQueueSubmit2(Context::GetLogicalDevice()->GetGraphicsQueue().queue, 1, &submitInfo, VK_NULL_HANDLE));
	}
	void Renderer::SubmitShadowCommands()
	{
		PROFILE_FUNCTION();
		CommandPool& commandPool = GetCommandPool(Context::GetFrameIndex(), RenderStage::shadow);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();

		// Wait semaphore info:
		VkSemaphoreSubmitInfo waitSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		waitSemaphoreInfo.semaphore = m_preRenderComputeToShadowSemaphores[Context::GetFrameIndex()];
		waitSemaphoreInfo.stageMask = PipelineStages::computeShader;

		// Command buffer info:
		VkCommandBufferSubmitInfo cmdBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
		cmdBufferInfo.commandBuffer = commandBuffer;

		// Signal semaphore info:
		VkSemaphoreSubmitInfo signalSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		signalSemaphoreInfo.semaphore = m_shadowToForwardSemaphores[Context::GetFrameIndex()];
		signalSemaphoreInfo.stageMask = PipelineStages::earlyFragmentTest | PipelineStages::lateFragmentTest;

		// Submit info:
		VkSubmitInfo2 submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
		submitInfo.waitSemaphoreInfoCount = 1;
		submitInfo.pWaitSemaphoreInfos = &waitSemaphoreInfo;
		submitInfo.commandBufferInfoCount = 1;
		submitInfo.pCommandBufferInfos = &cmdBufferInfo;
		submitInfo.signalSemaphoreInfoCount = 1;
		submitInfo.pSignalSemaphoreInfos = &signalSemaphoreInfo;

		// Submit:
		VKA(vkQueueSubmit2(Context::GetLogicalDevice()->GetGraphicsQueue().queue, 1, &submitInfo, VK_NULL_HANDLE));
	}
	void Renderer::SubmitForwardCommands()
	{
		PROFILE_FUNCTION();
		CommandPool& commandPool = GetCommandPool(Context::GetFrameIndex(), RenderStage::forward);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		
		// Wait semaphore info:
		VkSemaphoreSubmitInfo waitSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		waitSemaphoreInfo.semaphore = m_shadowToForwardSemaphores[Context::GetFrameIndex()];
		waitSemaphoreInfo.stageMask = PipelineStages::earlyFragmentTest | PipelineStages::lateFragmentTest;

		// Command buffer info:
		VkCommandBufferSubmitInfo cmdBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
		cmdBufferInfo.commandBuffer = commandBuffer;

		// Signal semaphore info:
		VkSemaphoreSubmitInfo signalSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		signalSemaphoreInfo.semaphore = m_forwardToPostRenderComputeSemaphores[Context::GetFrameIndex()];
		signalSemaphoreInfo.stageMask = PipelineStages::colorAttachmentOutput;

		// Submit info:
		VkSubmitInfo2 submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
		submitInfo.waitSemaphoreInfoCount = 1;
		submitInfo.pWaitSemaphoreInfos = &waitSemaphoreInfo;
		submitInfo.commandBufferInfoCount = 1;
		submitInfo.pCommandBufferInfos = &cmdBufferInfo;
		submitInfo.signalSemaphoreInfoCount = 1;
		submitInfo.pSignalSemaphoreInfos = &signalSemaphoreInfo;

		// Submit:
		VKA(vkQueueSubmit2(Context::GetLogicalDevice()->GetGraphicsQueue().queue, 1, &submitInfo, VK_NULL_HANDLE));
	}
	void Renderer::SubmitForwardCommandsParallel()
	{
		PROFILE_FUNCTION();
		CommandPool& commandPool = GetCommandPool(Context::GetFrameIndex(), RenderStage::forward);
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
			renderPassBeginInfo.framebuffer = RenderPassManager::GetForwardRenderPass()->GetFramebuffer(0);
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent.width = RenderPassManager::GetForwardRenderPass()->GetRenderTexture()->GetWidth();
			renderPassBeginInfo.renderArea.extent.height = RenderPassManager::GetForwardRenderPass()->GetRenderTexture()->GetHeight();
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
				memoryBarrier.srcStageMask = PipelineStages::vertexShader;
				memoryBarrier.dstStageMask = PipelineStages::computeShader;
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
		waitSemaphoreInfo.semaphore = m_shadowToForwardSemaphores[Context::GetFrameIndex()];
		waitSemaphoreInfo.stageMask = PipelineStages::earlyFragmentTest;

		// Command buffer info:
		VkCommandBufferSubmitInfo cmdBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
		cmdBufferInfo.commandBuffer = primaryCommandBuffer;

		// Signal semaphore info:
		VkSemaphoreSubmitInfo signalSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		signalSemaphoreInfo.semaphore = m_forwardToPostRenderComputeSemaphores[Context::GetFrameIndex()];
		signalSemaphoreInfo.stageMask = PipelineStages::computeShader;

		// Submit info:
		VkSubmitInfo2 submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
		submitInfo.waitSemaphoreInfoCount = 1;
		submitInfo.pWaitSemaphoreInfos = &waitSemaphoreInfo;
		submitInfo.commandBufferInfoCount = 1;
		submitInfo.pCommandBufferInfos = &cmdBufferInfo;
		submitInfo.signalSemaphoreInfoCount = 1;
		submitInfo.pSignalSemaphoreInfos = &signalSemaphoreInfo;

		// Submit:
		VKA(vkQueueSubmit2(Context::GetLogicalDevice()->GetGraphicsQueue().queue, 1, &submitInfo, VK_NULL_HANDLE));
	}
	void Renderer::SubmitPostRenderComputeCommands()
	{
		PROFILE_FUNCTION();
		CommandPool& commandPool = GetCommandPool(Context::GetFrameIndex(), RenderStage::postRenderCompute);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();

		// Wait semaphore info:
		VkSemaphoreSubmitInfo waitSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		waitSemaphoreInfo.semaphore = m_forwardToPostRenderComputeSemaphores[Context::GetFrameIndex()];
		waitSemaphoreInfo.stageMask = PipelineStages::computeShader;

		// Command buffer info:
		VkCommandBufferSubmitInfo cmdBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
		cmdBufferInfo.commandBuffer = commandBuffer;

		// Signal semaphore info:
		VkSemaphoreSubmitInfo signalSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		signalSemaphoreInfo.semaphore = m_postRenderToPresentSemaphores[Context::GetFrameIndex()];
		signalSemaphoreInfo.stageMask = PipelineStages::computeShader;

		// Submit info:
		VkSubmitInfo2 submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
		submitInfo.waitSemaphoreInfoCount = 1;
		submitInfo.pWaitSemaphoreInfos = &waitSemaphoreInfo;
		submitInfo.commandBufferInfoCount = 1;
		submitInfo.pCommandBufferInfos = &cmdBufferInfo;
		submitInfo.signalSemaphoreInfoCount = 1;
		submitInfo.pSignalSemaphoreInfos = &signalSemaphoreInfo;

		// Submit:
		VKA(vkQueueSubmit2(Context::GetLogicalDevice()->GetGraphicsQueue().queue, 1, &submitInfo, VK_NULL_HANDLE));
	}
	void Renderer::SubmitPresentCommands()
	{
		PROFILE_FUNCTION();
		CommandPool& commandPool = GetCommandPool(Context::GetFrameIndex(), RenderStage::present);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();

		// Wait semaphore info:
		VkSemaphoreSubmitInfo waitSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		waitSemaphoreInfo.semaphore = m_postRenderToPresentSemaphores[Context::GetFrameIndex()];
		waitSemaphoreInfo.stageMask = PipelineStages::colorAttachmentOutput;

		// Command buffer info:
		VkCommandBufferSubmitInfo cmdBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
		cmdBufferInfo.commandBuffer = commandBuffer;

		// Signal semaphore info:
		VkSemaphoreSubmitInfo signalSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		signalSemaphoreInfo.semaphore = m_releaseSemaphores[Context::GetFrameIndex()];
		signalSemaphoreInfo.stageMask = PipelineStages::bottomOfPipe;

		// Submit info:
		VkSubmitInfo2 submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
		submitInfo.waitSemaphoreInfoCount = 1;
		submitInfo.pWaitSemaphoreInfos = &waitSemaphoreInfo;
		submitInfo.commandBufferInfoCount = 1;
		submitInfo.pCommandBufferInfos = &cmdBufferInfo;
		submitInfo.signalSemaphoreInfoCount = 1;
		submitInfo.pSignalSemaphoreInfos = &signalSemaphoreInfo;

		// Submit:
		VKA(vkQueueSubmit2(Context::GetLogicalDevice()->GetGraphicsQueue().queue, 1, &submitInfo, m_frameFences[Context::GetFrameIndex()]));
	}
	bool Renderer::PresentImage()
	{
		PROFILE_FUNCTION();
		VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_releaseSemaphores[Context::GetFrameIndex()];
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &Context::GetVkSwapchainKHR();
		presentInfo.pImageIndices = &m_imageIndex;

		VkResult result = vkQueuePresentKHR(Context::GetLogicalDevice()->GetPresentQueue().queue, &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			m_rebuildSwapchain = true;
			return false;
		}
		return true;
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
			NAME_VK_FENCE(m_frameFences[i], "FrameFences" + std::to_string(i));
		}
	}
	void Renderer::CreateSemaphores()
	{
		VkSemaphoreCreateInfo createInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

        // One per frame in flight:
		m_acquireSemaphores.resize(Context::GetFramesInFlight());
		m_resourceUpdateToPreRenderComputeSemaphores.resize(Context::GetFramesInFlight());
		m_preRenderComputeToShadowSemaphores.resize(Context::GetFramesInFlight());
		m_shadowToForwardSemaphores.resize(Context::GetFramesInFlight());
		m_forwardToPostRenderComputeSemaphores.resize(Context::GetFramesInFlight());
		m_postRenderToPresentSemaphores.resize(Context::GetFramesInFlight());
		m_releaseSemaphores.resize(Context::GetFramesInFlight());
		for (uint32_t i = 0; i < Context::GetFramesInFlight(); i++)
		{
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_acquireSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_resourceUpdateToPreRenderComputeSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_preRenderComputeToShadowSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_shadowToForwardSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_forwardToPostRenderComputeSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_postRenderToPresentSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_releaseSemaphores[i]));
			NAME_VK_SEMAPHORE(m_acquireSemaphores[i], "AcquireSemaphore" + std::to_string(i));
			NAME_VK_SEMAPHORE(m_resourceUpdateToPreRenderComputeSemaphores[i], "ResourceUpdateToPreRenderComputeSemaphore" + std::to_string(i));
			NAME_VK_SEMAPHORE(m_preRenderComputeToShadowSemaphores[i], "PreRenderComputeToShadowSemaphore" + std::to_string(i));
			NAME_VK_SEMAPHORE(m_shadowToForwardSemaphores[i], "ShadowToForwardSemaphore" + std::to_string(i));
			NAME_VK_SEMAPHORE(m_forwardToPostRenderComputeSemaphores[i], "ForwardToPostRenderComputeSemaphore" + std::to_string(i));
			NAME_VK_SEMAPHORE(m_postRenderToPresentSemaphores[i], "PostRenderToPresentSemaphore" + std::to_string(i));
			NAME_VK_SEMAPHORE(m_releaseSemaphores[i], "ReleaseSemaphore" + std::to_string(i));
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
			vkDestroySemaphore(Context::GetVkDevice(), m_preRenderComputeToShadowSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_shadowToForwardSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_forwardToPostRenderComputeSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_postRenderToPresentSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_releaseSemaphores[i], nullptr);
		}
		m_acquireSemaphores.clear();
		m_resourceUpdateToPreRenderComputeSemaphores.clear();
		m_preRenderComputeToShadowSemaphores.clear();
		m_shadowToForwardSemaphores.clear();
		m_forwardToPostRenderComputeSemaphores.clear();
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