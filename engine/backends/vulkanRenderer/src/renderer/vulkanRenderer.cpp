#include "vulkanRenderer.h"
#include "emberMath.h"
#include "iDearImGui.h"
#include "profiler.h"
#include "spirvReflect.h"
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
#include "vulkanDefaultGpuResources.h"
#include "vulkanDefaultPushConstant.h"
#include "vulkanDrawCall.h"
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
#include "vulkanShaderProperties.h"
#include "vulkanShadowPushConstant.h"
#include "vulkanShadowRenderPass.h"
#include "vulkanStorageBuffer.h"
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
	Renderer::Renderer(const emberCommon::RendererCreateInfo& createInfo)
	{
		Context::Init(createInfo);

		m_time = 0.0f;
		m_deltaTime = 0.0f;
		m_rebuildSwapchain = false;
		std::filesystem::path directoryPath = (std::filesystem::path(VULKAN_LIBRARY_ROOT_PATH) / "src" / "shaders").make_preferred();

		// Command pools (one per frameInFlight * renderStage):
		m_commandPools.reserve(Context::GetFramesInFlight() * (int)RenderStage::stageCount);
		for (int i = 0; i < Context::GetFramesInFlight() * (int)RenderStage::stageCount; i++)
			m_commandPools.emplace_back(emberTaskSystem::TaskSystem::GetCoreCount(), Context::GetLogicalDevice()->GetGraphicsQueue());

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
		m_pShadowMaterial = std::make_unique<Material>(m_shadowMapResolution);
		m_shadowPipelineLayout = m_pShadowMaterial->GetPipeline()->GetVkPipelineLayout();

		// Present render pass caching:
		m_pPresentMesh = std::unique_ptr<Mesh>(CreateFullScreenRenderQuad());
		m_pPresentMaterial = std::make_unique<Material>(emberCommon::MaterialType::present, "presentMaterial", emberCommon::RenderQueue::opaque, directoryPath / "present.vert.spv", directoryPath / "present.frag.spv");
		m_pPresentShaderProperties = std::make_unique<ShaderProperties>((Shader*)m_pPresentMaterial.get());
		m_presentPipeline = m_pPresentMaterial->GetPipeline()->GetVkPipeline();
		m_presentPipelineLayout = m_pPresentMaterial->GetPipeline()->GetVkPipelineLayout();
		m_presentBindingCount = m_pPresentMaterial->GetVertexInputDescriptions()->size;

		m_pGammaCorrectionComputeShader = std::make_unique<ComputeShader>("gammaCorrectionComputeShader", directoryPath / "gammaCorrection.comp.spv");

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
		GarbageCollector::CollectGarbage();
		DestroySemaphores();
		DestroyFences();
		Context::Clear();
	}



	// Movable:
	Renderer::Renderer(Renderer&& other) noexcept = default;
	Renderer& Renderer::operator=(Renderer&& other) noexcept = default;



	// Main render call:
	void Renderer::RenderFrame(int windowWidth, int windowHeight, float time, float deltaTime)
	{
		m_time = time;
		m_deltaTime = deltaTime;

		// Resize Swapchain if needed:
		Uint2 surfaceExtend = Context::GetSurface()->GetCurrentExtent();
		if (m_rebuildSwapchain || windowWidth != surfaceExtend.x || windowHeight != surfaceExtend.y)
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
		
		// Record and submit current frame commands:
		{
			PROFILE_SCOPE("Record");
			DEBUG_LOG_CRITICAL("Recording frame {}", Context::GetFrameIndex());

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
	void Renderer::DrawMesh(emberBackendInterface::IMesh* pMesh, emberBackendInterface::IMaterial* pMaterial, emberBackendInterface::IShaderProperties* pShaderProperties, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows)
	{
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

		// Setup draw call:
		ShaderProperties* pShadowShaderProperties = PoolManager::CheckOutShaderProperties((Shader*)m_pShadowMaterial.get());
		DrawCall drawCall = { localToWorldMatrix, receiveShadows, castShadows, static_cast<Material*>(pMaterial), static_cast<ShaderProperties*>(pShaderProperties), pShadowShaderProperties, static_cast<Mesh*>(pMesh), 0 };
		m_staticDrawCalls.push_back(drawCall);
	}
	emberBackendInterface::IShaderProperties* Renderer::DrawMesh(emberBackendInterface::IMesh* pMesh, emberBackendInterface::IMaterial* pMaterial, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows)
	{
		ShaderProperties* pShaderProperties = PoolManager::CheckOutShaderProperties(static_cast<Shader*>(static_cast<Material*>(pMaterial)));
		DrawMesh(pMesh, pMaterial, pShaderProperties, localToWorldMatrix, receiveShadows, castShadows);
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
	float Renderer::GetDeptBiasConstantFactor()
	{
		return m_depthBiasConstantFactor;
	}
	float Renderer::GetDeptBiasClamp()
	{
		return m_depthBiasClamp;
	}
	float Renderer::GetDeptBiasSlopeFactor()
	{
		return m_depthBiasSlopeFactor;
	}



	// Setters:
	void Renderer::SetIComputeHandle(emberBackendInterface::ICompute* pICompute)
	{
		m_pCompute = static_cast<Compute*>(pICompute);
	}
	void Renderer::SetIDearImGuiHandle(emberBackendInterface::IDearImGui* pIDearImGui)
	{
		m_pIDearImGui = pIDearImGui;
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
		return static_cast<void*>(DefaultGpuResources::GetColorSampler());
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
			PoolManager::ReturnShaderProperties((Shader*)m_pShadowMaterial.get(), drawCall.pShadowShaderProperties);

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
		//if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || Context::pWindow->GetFramebufferResized())
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			//Context::pWindow->SetFramebufferResized(false);
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

		// Sort draw call pointers according to material renderQueue:
		std::sort(m_sortedDrawCallPointers.begin(), m_sortedDrawCallPointers.end(), [](DrawCall* a, DrawCall* b)
		{
			return a->pMaterial->GetRenderQueue() < b->pMaterial->GetRenderQueue();
		});
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
		GetCommandPool(Context::GetFrameIndex(), RenderStage::preRenderCompute).ResetPools();
		GetCommandPool(Context::GetFrameIndex(), RenderStage::shadow).ResetPools();
		GetCommandPool(Context::GetFrameIndex(), RenderStage::forward).ResetPools();
		GetCommandPool(Context::GetFrameIndex(), RenderStage::postRenderCompute).ResetPools();
		GetCommandPool(Context::GetFrameIndex(), RenderStage::present).ResetPools();
	}



	// Record commands:
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
					// Update shader specific data:
					computeCall->pShaderProperties->UpdateShaderData();

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
				DEBUG_LOG_TRACE("Memory Barrier: compute to vertex");
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
			const VkDeviceSize offsets[1] = { 0 };

			// Begin render pass:
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			{
				// BestPractices validation layer warning occur when binding a pipeline with vertex input data, but not binding any vertex buffer.
				if (m_directionalLightsCount > 0 || m_positionalLightsCount > 0)
				{
					int shadowMapIndex = 0;
					vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pShadowMaterial->GetPipeline()->GetVkPipeline());

					// Dynamic state: depth bias:
					vkCmdSetDepthBias(commandBuffer, m_depthBiasConstantFactor, m_depthBiasClamp, m_depthBiasSlopeFactor);

					// Update shader specific data:
					for (DrawCall* drawCall : m_sortedDrawCallPointers)
						drawCall->pShadowShaderProperties->UpdateShaderData();

					// Directional Lights:
					for (int i = 0; i < m_directionalLightsCount; i++)
					{
						for (DrawCall* drawCall : m_sortedDrawCallPointers)
						{
							if (drawCall->castShadows == false)
								continue;

							Mesh* pMesh = drawCall->pMesh;

							ShadowPushConstant pushConstant(drawCall->instanceCount, shadowMapIndex, drawCall->localToWorldMatrix, m_directionalLights[i].worldToClipMatrix);
							vkCmdPushConstants(commandBuffer, m_shadowPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ShadowPushConstant), &pushConstant);

							vkCmdBindVertexBuffers(commandBuffer, 0, 1, &pMesh->GetVertexBuffer()->GetVmaBuffer()->GetVkBuffer(), offsets);
							vkCmdBindIndexBuffer(commandBuffer, pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, static_cast<VkIndexType>(Mesh::GetIndexType()));

							vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_shadowPipelineLayout, 0, 1, &drawCall->pShadowShaderProperties->GetDescriptorSet(Context::GetFrameIndex()), 0, nullptr);
							vkCmdDrawIndexed(commandBuffer, 3 * pMesh->GetTriangleCount(), std::max(drawCall->instanceCount, (uint32_t)1), 0, 0, 0);
							DEBUG_LOG_INFO("Directional light, mesh = {}", drawCall->pMesh->GetName());
						}
						shadowMapIndex++;
					}

					// Positional Lights:
					for (int i = 0; i < m_positionalLightsCount; i++)
					{
						for (DrawCall* drawCall : m_sortedDrawCallPointers)
						{
							if (drawCall->castShadows == false)
								continue;

							Mesh* pMesh = drawCall->pMesh;

							ShadowPushConstant pushConstant(drawCall->instanceCount, shadowMapIndex, drawCall->localToWorldMatrix, m_positionalLights[i].worldToClipMatrix);
							vkCmdPushConstants(commandBuffer, m_shadowPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ShadowPushConstant), &pushConstant);

							vkCmdBindVertexBuffers(commandBuffer, 0, 1, &pMesh->GetVertexBuffer()->GetVmaBuffer()->GetVkBuffer(), offsets);
							vkCmdBindIndexBuffer(commandBuffer, pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, static_cast<VkIndexType>(Mesh::GetIndexType()));

							vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_shadowPipelineLayout, 0, 1, &drawCall->pShadowShaderProperties->GetDescriptorSet(Context::GetFrameIndex()), 0, nullptr);
							vkCmdDrawIndexed(commandBuffer, 3 * pMesh->GetTriangleCount(), std::max(drawCall->instanceCount, (uint32_t)1), 0, 0, 0);
							DEBUG_LOG_INFO("Positional light, mesh = {}", drawCall->pMesh->GetName());
						}
						shadowMapIndex++;
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
				uint32_t bindingCount = 0;
				Mesh* pMesh = nullptr;
				Material* pMaterial = nullptr;
				Material* pPreviousMaterial = nullptr;
				VkPipeline pipeline = VK_NULL_HANDLE;
				VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
				DefaultPushConstant pushConstant(0, m_time, m_deltaTime, m_directionalLightsCount, m_positionalLightsCount, m_activeCamera.position);

				// Normal draw calls:
				for (DrawCall* drawCall : m_sortedDrawCallPointers)
				{
					PROFILE_SCOPE("DrawCall");
					pMesh = drawCall->pMesh;

					// Update shader specific data:
					drawCall->SetRenderMatrizes(m_activeCamera.viewMatrix, m_activeCamera.projectionMatrix);
					drawCall->SetLightData(m_directionalLights, m_positionalLights);
					drawCall->pShaderProperties->UpdateShaderData();

					// Change pipeline if material has changed:
					pMaterial = drawCall->pMaterial;
					if (pPreviousMaterial != pMaterial)
					{
						pPreviousMaterial = pMaterial;
						pipeline = pMaterial->GetPipeline()->GetVkPipeline();
						pipelineLayout = pMaterial->GetPipeline()->GetVkPipelineLayout();
						bindingCount = pMaterial->GetVertexInputDescriptions()->size;
						pushConstant.instanceCount = drawCall->instanceCount;
						vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
						vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(DefaultPushConstant), &pushConstant);
					}

					// Same material but different instance Count => update push constants:
					if (pushConstant.instanceCount != drawCall->instanceCount)
					{
						pushConstant.instanceCount = drawCall->instanceCount;
						vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(DefaultPushConstant), &pushConstant);
					}

					vkCmdBindVertexBuffers(commandBuffer, 0, bindingCount, pMaterial->GetMeshBuffers(pMesh), pMaterial->GetMeshOffsets(pMesh));
					vkCmdBindIndexBuffer(commandBuffer, pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, static_cast<VkIndexType>(Mesh::GetIndexType()));
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &drawCall->pShaderProperties->GetDescriptorSet(Context::GetFrameIndex()), 0, nullptr);
					vkCmdDrawIndexed(commandBuffer, 3 * pMesh->GetTriangleCount(), std::max(drawCall->instanceCount, (uint32_t)1), 0, 0, 0);
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

			// Record commands: (no beign renderpass for secondary command buffers)
			{
				uint32_t bindingCount = 0;
				Mesh* pMesh = nullptr;
				Material* pMaterial = nullptr;
				Material* pPreviousMaterial = nullptr;
				VkPipeline pipeline = VK_NULL_HANDLE;
				VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
				DefaultPushConstant pushConstant(0, m_time, m_deltaTime, m_directionalLightsCount, m_positionalLightsCount, m_activeCamera.position);

				// Normal draw calls:
				for (int i = startIndex; i < endIndex; i++)
				{
					DrawCall* drawCall = (m_sortedDrawCallPointers)[i];
					pMesh = drawCall->pMesh;

					// Update shader specific data:
					drawCall->SetRenderMatrizes(m_activeCamera.viewMatrix, m_activeCamera.projectionMatrix);
					drawCall->SetLightData(m_directionalLights, m_positionalLights);
					drawCall->pShaderProperties->UpdateShaderData();

					// Change pipeline if material has changed:
					pMaterial = drawCall->pMaterial;
					if (pPreviousMaterial != pMaterial)
					{
						pPreviousMaterial = pMaterial;
						pipeline = pMaterial->GetPipeline()->GetVkPipeline();
						pipelineLayout = pMaterial->GetPipeline()->GetVkPipelineLayout();
						bindingCount = pMaterial->GetVertexInputDescriptions()->size;
						pushConstant.instanceCount = drawCall->instanceCount;
						vkCmdBindPipeline(secondaryCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
						vkCmdPushConstants(secondaryCommandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(DefaultPushConstant), &pushConstant);
					}

					// Same material but different instance Count => update push constants:
					if (pushConstant.instanceCount != drawCall->instanceCount)
					{
						pushConstant.instanceCount = drawCall->instanceCount;
						vkCmdPushConstants(secondaryCommandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(DefaultPushConstant), &pushConstant);
					}

					vkCmdBindVertexBuffers(secondaryCommandBuffer, 0, bindingCount, pMaterial->GetMeshBuffers(pMesh), pMaterial->GetMeshOffsets(pMesh));
					vkCmdBindIndexBuffer(secondaryCommandBuffer, pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, static_cast<VkIndexType>(Mesh::GetIndexType()));

					vkCmdBindDescriptorSets(secondaryCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &drawCall->pShaderProperties->GetDescriptorSet(Context::GetFrameIndex()), 0, nullptr);
					vkCmdDrawIndexed(secondaryCommandBuffer, 3 * pMesh->GetTriangleCount(), std::max(drawCall->instanceCount, (uint32_t)1), 0, 0, 0);
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

			uint32_t callIndex = 0;
			for (ComputeCall* computeCall : m_pCompute->GetPostRenderCompute()->GetComputeCallPointers())
			{
				// Update shader specific data:
				if (callIndex % 2 == 0)
				{
					computeCall->pShaderProperties->SetTexture("inputImage", RenderPassManager::GetForwardRenderPass()->GetRenderTexture());
					computeCall->pShaderProperties->SetTexture("outputImage", RenderPassManager::GetForwardRenderPass()->GetSecondaryRenderTexture());
				}
				else
				{
					computeCall->pShaderProperties->SetTexture("inputImage", RenderPassManager::GetForwardRenderPass()->GetSecondaryRenderTexture());
					computeCall->pShaderProperties->SetTexture("outputImage", RenderPassManager::GetForwardRenderPass()->GetRenderTexture());
				}
				callIndex++;
				computeCall->pShaderProperties->UpdateShaderData();

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
				DEBUG_LOG_ERROR("Render Image Transition: ??? -> shader read only");
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
				// Update shader specific data:
				m_pPresentShaderProperties->SetTexture("renderTexture", RenderPassManager::GetForwardRenderPass()->GetRenderTexture());
				m_pPresentShaderProperties->UpdateShaderData();

				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_presentPipeline);

				vkCmdBindVertexBuffers(commandBuffer, 0, m_presentBindingCount, m_pPresentMaterial->GetMeshBuffers(m_pPresentMesh.get()), m_pPresentMaterial->GetMeshOffsets(m_pPresentMesh.get()));
				vkCmdBindIndexBuffer(commandBuffer, m_pPresentMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, static_cast<VkIndexType>(Mesh::GetIndexType()));

				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_presentPipelineLayout, 0, 1, &m_pPresentShaderProperties->GetDescriptorSet(Context::GetFrameIndex()), 0, nullptr);
				vkCmdDrawIndexed(commandBuffer, 3 * m_pPresentMesh->GetTriangleCount(), 1, 0, 0, 0);
				DEBUG_LOG_INFO("Render renderTexture into fullScreenRenderQuad, material = {}", m_pPresentMaterial->GetName());
				
				m_pIDearImGui->Render(commandBuffer);
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
				m_pIDearImGui->Render(commandBuffer);
			}
			vkCmdEndRenderPass(commandBuffer);
		}
		VKA(vkEndCommandBuffer(commandBuffer));
	}



	// Submit commands:
	void Renderer::SubmitPreRenderComputeCommands()
	{
		PROFILE_FUNCTION();
		CommandPool& commandPool = GetCommandPool(Context::GetFrameIndex(), RenderStage::preRenderCompute);
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
		else
		{
			VKA(result);
			return true;
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
			NAME_VK_FENCE(m_frameFences[i], "FrameFences" + std::to_string(i));
		}
	}
	void Renderer::CreateSemaphores()
	{
		VkSemaphoreCreateInfo createInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

        // One per frame in flight:
		m_acquireSemaphores.resize(Context::GetFramesInFlight());
		m_preRenderComputeToShadowSemaphores.resize(Context::GetFramesInFlight());
		m_shadowToForwardSemaphores.resize(Context::GetFramesInFlight());
		m_forwardToPostRenderComputeSemaphores.resize(Context::GetFramesInFlight());
		m_postRenderToPresentSemaphores.resize(Context::GetFramesInFlight());
		m_releaseSemaphores.resize(Context::GetFramesInFlight());
		for (uint32_t i = 0; i < Context::GetFramesInFlight(); i++)
		{
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_acquireSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_preRenderComputeToShadowSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_shadowToForwardSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_forwardToPostRenderComputeSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_postRenderToPresentSemaphores[i]));
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &m_releaseSemaphores[i]));
			NAME_VK_SEMAPHORE(m_acquireSemaphores[i], "AcquireSemaphore" + std::to_string(i));
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
			vkDestroySemaphore(Context::GetVkDevice(), m_preRenderComputeToShadowSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_shadowToForwardSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_forwardToPostRenderComputeSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_postRenderToPresentSemaphores[i], nullptr);
			vkDestroySemaphore(Context::GetVkDevice(), m_releaseSemaphores[i], nullptr);
		}
		m_acquireSemaphores.clear();
		m_preRenderComputeToShadowSemaphores.clear();
		m_shadowToForwardSemaphores.clear();
		m_forwardToPostRenderComputeSemaphores.clear();
		m_postRenderToPresentSemaphores.clear();
		m_releaseSemaphores.clear();
	}
	
	
	
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



	Mesh* Renderer::CreateFullScreenRenderQuad()
	{
		Mesh* pMesh = new Mesh("fullScreenRenderQuad");

		std::vector<Float3> positions;
		positions.emplace_back(-1.0f, -1.0f, 0.0f);
		positions.emplace_back(-1.0f, 1.0f, 0.0f);
		positions.emplace_back(1.0f, -1.0f, 0.0f);
		positions.emplace_back(1.0f, 1.0f, 0.0f);

		std::vector<Float3> normals;
		normals.emplace_back(0.0f, 0.0f, 1.0f);
		normals.emplace_back(0.0f, 0.0f, 1.0f);
		normals.emplace_back(0.0f, 0.0f, 1.0f);
		normals.emplace_back(0.0f, 0.0f, 1.0f);

		std::vector<Float4> uvs;
		uvs.emplace_back(0.0f, 0.0f, 0.0f, 0.0f);
		uvs.emplace_back(0.0f, 1.0f, 0.0f, 0.0f);
		uvs.emplace_back(1.0f, 0.0f, 0.0f, 0.0f);
		uvs.emplace_back(1.0f, 1.0f, 0.0f, 0.0f);

		std::vector<Uint3> triangles;
		triangles.emplace_back(Uint3(0, 2, 1));
		triangles.emplace_back(Uint3(1, 2, 3));

		pMesh->MovePositions(std::move(positions));
		pMesh->MoveNormals(std::move(normals));
		pMesh->MoveUVs(std::move(uvs));
		pMesh->MoveTriangles(std::move(triangles));
		pMesh->ComputeTangents();
		return pMesh;
	}
}