#include "vulkanRenderer.h"
#include "emberMath.h"
#include "iDearImGui.h"
#include "profiler.h"
#include "rendererCreateInfo.h"
#include "spirvReflect.h"
#include "taskSystem.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanAccessMasks.h"
#include "vulkanCommandPool.h"
#include "vulkanCompute.h"
#include "vulkanComputeCall.h"
#include "vulkanComputePushConstant.h"
#include "vulkanComputeShader.h"
#include "vulkanContext.h"
#include "vulkanDefaultPushConstant.h"
#include "vulkanDrawCall.h"
#include "vulkanForwardRenderPass.h"
#include "vulkanGraphics.h"
#include "vulkanIndexBuffer.h"
#include "vulkanLighting.h"
#include "vulkanMacros.h"
#include "vulkanMaterial.h"
#include "vulkanMesh.h"
#include "vulkanPipeline.h"
#include "vulkanPipelineStages.h"
#include "vulkanPostRenderCompute.h"
#include "vulkanPreRenderCompute.h"
#include "vulkanPresentRenderPass.h"
#include "vulkanRenderPassManager.h"
#include "vulkanRenderTexture2d.h"
#include "vulkanShaderProperties.h"
#include "vulkanShadowPushConstant.h"
#include "vulkanShadowRenderPass.h"
#include "vulkanVertexBuffer.h"
#include <assert.h>
#include <string>



namespace vulkanRendererBackend
{
	// Constructor/Destructor:
	Renderer::Renderer(const emberEngine::RendererCreateInfo& createInfo)
	{
		// Assert existence of required interfaces:
		assert(createInfo.pIDearImGui != nullptr);
		assert(createInfo.pIDearImGui != nullptr);

		Context::Init(createInfo);

		m_pIDearImGui = createInfo.pIDearImGui;
		m_time = 0.0f;
		m_deltaTime = 0.0f;
		m_rebuildSwapchain = false;

		// Command pools (one per frameInFlight * renderStage):
		m_commandPools.reserve(Context::framesInFlight * (int)RenderStage::stageCount);
		for (int i = 0; i < Context::framesInFlight * (int)RenderStage::stageCount; i++)
			m_commandPools.emplace_back(emberTaskSystem::TaskSystem::GetCoreCount(), Context::logicalDevice.GetGraphicsQueue());

		// Shadow render pass caching:
		m_shadowPipeline = Graphics::GetShadowMaterial()->GetPipeline()->GetVkPipeline();
		m_shadowPipelineLayout = Graphics::GetShadowMaterial()->GetPipeline()->GetVkPipelineLayout();

		// Present render pass caching:
		m_pPresentMesh = Graphics::GetFullScreenRenderQuad();
		m_pPresentMaterial = Graphics::GetPresentMaterial();
		m_pPresentShaderProperties = std::make_unique<ShaderProperties>((Shader*)m_pPresentMaterial);
		m_presentPipeline = m_pPresentMaterial->GetPipeline()->GetVkPipeline();
		m_presentPipelineLayout = m_pPresentMaterial->GetPipeline()->GetVkPipelineLayout();
		m_presentBindingCount = m_pPresentMaterial->GetVertexInputDescriptions()->size;

		// Debug naming:
		for (int renderStage = 0; renderStage < (int)RenderStage::stageCount; renderStage++)
			for (int frameIndex = 0; frameIndex < Context::framesInFlight; frameIndex++)
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
		Context::Clear();
	}



	// Public methods:
	void Renderer::RenderFrame(int windowWidth, int windowHeight, float time, float deltaTime)
	{
		m_time = time;
		m_deltaTime = deltaTime;

		// Resize Swapchain if needed:
		VkExtent2D surfaceExtend = Context::surface.GetCurrentExtent();
		if (m_rebuildSwapchain || windowWidth != surfaceExtend.width || windowHeight != surfaceExtend.height)
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

		// I use linear color space throughout entire render process. So apply gamma correction in post processing:
		ComputeShader* pComputeShader = Graphics::GetGammaCorrectionComputeShader();
		PostRender::RecordComputeShader(pComputeShader);
		
		// Record and submit current frame commands:
		{
			PROFILE_SCOPE("Record");
			DEBUG_LOG_CRITICAL("Recording frame {}", Context::frameIndex);

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

			if (!Context::enableDockSpace)
				RecordPresentCommands();
			else
				RecordImGuiPresentCommands();
			SubmitPresentCommands();
		}

		// Reset engine data for next frame:
		PreRender::ResetComputeCalls();
		Lighting::ResetLights();
		Graphics::ResetDrawCalls();
		PostRender::ResetComputeCalls();

		// Cancel current frame on failed presentation (e.g. window resize):
		if (!PresentImage())
			return;

		Context::UpdateFrameIndex();
	}



	// Private methods:
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
		VkResult result = vkAcquireNextImageKHR(Context::GetVkDevice(), Context::GetVkSwapchainKHR(), UINT64_MAX, m_acquireSemaphores[Context::frameIndex], VK_NULL_HANDLE, &m_imageIndex);

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

	// Wait for fence:
	void Renderer::WaitForFrameFence()
	{
		PROFILE_FUNCTION();
		VKA(vkWaitForFences(Context::GetVkDevice(), 1, &m_frameFences[Context::frameIndex], VK_TRUE, UINT64_MAX));
		VKA(vkResetFences(Context::GetVkDevice(), 1, &m_frameFences[Context::frameIndex]));
	}
	void Renderer::ResetCommandPools()
	{
		GetCommandPool(Context::frameIndex, RenderStage::preRenderCompute).ResetPools();
		GetCommandPool(Context::frameIndex, RenderStage::shadow).ResetPools();
		GetCommandPool(Context::frameIndex, RenderStage::forward).ResetPools();
		GetCommandPool(Context::frameIndex, RenderStage::postRenderCompute).ResetPools();
		GetCommandPool(Context::frameIndex, RenderStage::present).ResetPools();
	}

	// Record commands:
	void Renderer::RecordPreRenderComputeCommands()
	{
		PROFILE_FUNCTION();

		// Prepare command recording:
		CommandPool& commandPool = GetCommandPool(Context::frameIndex, RenderStage::preRenderCompute);
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

			for (ComputeCall* computeCall : PreRender::GetComputeCallPointers())
			{
				// Compute call is a barrier:
				if (computeCall->pComputeShader == nullptr)
				{
					VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
					memoryBarrier.srcStageMask = pipelineStage::computeShader;
					memoryBarrier.dstStageMask = pipelineStage::computeShader;
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

					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &computeCall->pShaderProperties->GetDescriptorSet(Context::frameIndex), 0, nullptr);
					vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
					DEBUG_LOG_TRACE("Pre Render Compute Shader {}, callIndex = {}", computeCall->pComputeShader->GetName(), computeCall->callIndex);
				}
			}

			// Release memory from compute shaders to vertex shaders:
			{
				VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
				memoryBarrier.srcStageMask = pipelineStage::computeShader;
				memoryBarrier.dstStageMask = pipelineStage::vertexShader;
				memoryBarrier.srcAccessMask = accessMask::computeShader::shaderWrite;
				memoryBarrier.dstAccessMask = accessMask::vertexShader::shaderRead;

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
		std::vector<DrawCall*> drawCalls = Graphics::GetSortedDrawCallPointers();
		CommandPool& commandPool = GetCommandPool(Context::frameIndex, RenderStage::shadow);
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
			renderPassBeginInfo.renderArea.extent = VkExtent2D{ Lighting::GetShadowMapResolution(), Lighting::GetShadowMapResolution() };
			renderPassBeginInfo.clearValueCount = 1;
			renderPassBeginInfo.pClearValues = &clearValues;
			const VkDeviceSize offsets[1] = { 0 };

			// Begin render pass:
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			{
				// BestPractices validation layer warning occur when binding a pipeline with vertex input data, but not binding any vertex buffer.
				if (Lighting::GetDirectionalLightsCount() > 0 || Lighting::GetPositionalLightsCount() > 0)
				{
					int shadowMapIndex = 0;
					vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_shadowPipeline);

					// Dynamic state: depth bias:
					vkCmdSetDepthBias(commandBuffer, Graphics::GetDeptBiasConstantFactor(), Graphics::GetDeptBiasClamp(), Graphics::GetDeptBiasSlopeFactor());

					// Update shader specific data:
					for (DrawCall* drawCall : drawCalls)
						drawCall->pShadowShaderProperties->UpdateShaderData();

					// Directional Lights:
					std::vector<Lighting::DirectionalLight>& directionalLights = Lighting::GetDirectionalLights();
					for (int i = 0; i < Lighting::GetDirectionalLightsCount(); i++)
					{
						Lighting::DirectionalLight& light = directionalLights[i];
						for (DrawCall* drawCall : drawCalls)
						{
							if (drawCall->castShadows == false)
								continue;

							Mesh* pMesh = drawCall->pMesh;

							ShadowPushConstant pushConstant(drawCall->instanceCount, shadowMapIndex, drawCall->localToWorldMatrix, light.worldToClipMatrix);
							vkCmdPushConstants(commandBuffer, m_shadowPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ShadowPushConstant), &pushConstant);

							vkCmdBindVertexBuffers(commandBuffer, 0, 1, &pMesh->GetVertexBuffer()->GetVmaBuffer()->GetVkBuffer(), offsets);
							vkCmdBindIndexBuffer(commandBuffer, pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, Mesh::GetIndexType());

							vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_shadowPipelineLayout, 0, 1, &drawCall->pShadowShaderProperties->GetDescriptorSet(Context::frameIndex), 0, nullptr);
							vkCmdDrawIndexed(commandBuffer, 3 * pMesh->GetTriangleCount(), std::max(drawCall->instanceCount, (uint32_t)1), 0, 0, 0);
							DEBUG_LOG_INFO("Directional light, mesh = {}", drawCall->pMesh->GetName());
						}
						shadowMapIndex++;
					}

					// Positional Lights:
					std::vector<Lighting::PositionalLight>& positionalLights = Lighting::GetPositionalLights();
					for (int i = 0; i < Lighting::GetPositionalLightsCount(); i++)
					{
						Lighting::PositionalLight& light = positionalLights[i];
						for (DrawCall* drawCall : drawCalls)
						{
							if (drawCall->castShadows == false)
								continue;

							Mesh* pMesh = drawCall->pMesh;

							ShadowPushConstant pushConstant(drawCall->instanceCount, shadowMapIndex, drawCall->localToWorldMatrix, light.worldToClipMatrix);
							vkCmdPushConstants(commandBuffer, m_shadowPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ShadowPushConstant), &pushConstant);

							vkCmdBindVertexBuffers(commandBuffer, 0, 1, &pMesh->GetVertexBuffer()->GetVmaBuffer()->GetVkBuffer(), offsets);
							vkCmdBindIndexBuffer(commandBuffer, pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, Mesh::GetIndexType());

							vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_shadowPipelineLayout, 0, 1, &drawCall->pShadowShaderProperties->GetDescriptorSet(Context::frameIndex), 0, nullptr);
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
		std::vector<DrawCall*> drawCalls = Graphics::GetSortedDrawCallPointers();
		CommandPool& commandPool = GetCommandPool(Context::frameIndex, RenderStage::forward);
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
				DefaultPushConstant pushConstant(0, m_time, m_deltaTime, Lighting::GetDirectionalLightsCount(), Lighting::GetPositionalLightsCount(), Graphics::GetActiveCamera().position);

				// Normal draw calls:
				for (DrawCall* drawCall : drawCalls)
				{
					PROFILE_SCOPE("DrawCall");
					pMesh = drawCall->pMesh;

					// Update shader specific data:
					drawCall->SetRenderMatrizes(Graphics::GetActiveCamera().viewMatrix, Graphics::GetActiveCamera().projectionMatrix);
					drawCall->SetLightData();
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
					vkCmdBindIndexBuffer(commandBuffer, pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, Mesh::GetIndexType());
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &drawCall->pShaderProperties->GetDescriptorSet(Context::frameIndex), 0, nullptr);
					vkCmdDrawIndexed(commandBuffer, 3 * pMesh->GetTriangleCount(), std::max(drawCall->instanceCount, (uint32_t)1), 0, 0, 0);
					DEBUG_LOG_WARN("Forward draw call, mesh = {}, material = {}", drawCall->pMesh->GetName(), drawCall->pMaterial->GetName());
				}
			}
			vkCmdEndRenderPass(commandBuffer);

			// Release memory from vertex shaders to compute shaders:
			{
				VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
				memoryBarrier.srcStageMask = pipelineStage::vertexShader;
				memoryBarrier.dstStageMask = pipelineStage::computeShader;
				memoryBarrier.srcAccessMask = accessMask::vertexShader::shaderRead;
				memoryBarrier.dstAccessMask = accessMask::computeShader::shaderWrite;

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
		std::vector<DrawCall*> drawCalls = Graphics::GetSortedDrawCallPointers();
		int totalWorkload = (int)drawCalls.size();
		int threadIndex = emberTaskSystem::TaskSystem::GetThreadIndex();
		int coreCount = emberTaskSystem::TaskSystem::GetCoreCount();
		int baseChunkSize = totalWorkload / coreCount;
		int remainder = totalWorkload % coreCount;
		int startIndex = threadIndex * baseChunkSize + std::min(threadIndex, remainder);
		int endIndex = startIndex + baseChunkSize + (threadIndex < remainder ? 1 : 0);

		// Prepare command recording:
		CommandPool& commandPool = GetCommandPool(Context::frameIndex, RenderStage::forward);
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
				DefaultPushConstant pushConstant(0, m_time, m_deltaTime, Lighting::GetDirectionalLightsCount(), Lighting::GetPositionalLightsCount(), Graphics::GetActiveCamera().position);

				// Normal draw calls:
				for (int i = startIndex; i < endIndex; i++)
				{
					DrawCall* drawCall = (drawCalls)[i];
					pMesh = drawCall->pMesh;

					// Update shader specific data:
					drawCall->SetRenderMatrizes(Graphics::GetActiveCamera().viewMatrix, Graphics::GetActiveCamera().projectionMatrix);
					drawCall->SetLightData();
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
					vkCmdBindIndexBuffer(secondaryCommandBuffer, pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, Mesh::GetIndexType());

					vkCmdBindDescriptorSets(secondaryCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &drawCall->pShaderProperties->GetDescriptorSet(Context::frameIndex), 0, nullptr);
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
		CommandPool& commandPool = GetCommandPool(Context::frameIndex, RenderStage::postRenderCompute);
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
			for (ComputeCall* computeCall : PostRender::GetComputeCallPointers())
			{
				// Update shader specific data:
				if (callIndex % 2 == 0)
				{
					computeCall->pShaderProperties->SetTexture2d("inputImage", RenderPassManager::GetForwardRenderPass()->GetRenderTexture());
					computeCall->pShaderProperties->SetTexture2d("outputImage", RenderPassManager::GetForwardRenderPass()->GetSecondaryRenderTexture());
				}
				else
				{
					computeCall->pShaderProperties->SetTexture2d("inputImage", RenderPassManager::GetForwardRenderPass()->GetSecondaryRenderTexture());
					computeCall->pShaderProperties->SetTexture2d("outputImage", RenderPassManager::GetForwardRenderPass()->GetRenderTexture());
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

				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &computeCall->pShaderProperties->GetDescriptorSet(Context::frameIndex), 0, nullptr);
				vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
				DEBUG_LOG_ERROR("Post Render Compute Shader {}, callIndex = {}", computeCall->pComputeShader->GetName(), computeCall->callIndex);

				// All effects access the same inputImage and outputImage.
				// Thus add memory barrier between every effect:
				{
					VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
					memoryBarrier.srcStageMask = pipelineStage::computeShader;
					memoryBarrier.dstStageMask = pipelineStage::computeShader;
					memoryBarrier.srcAccessMask = accessMask::computeShader::shaderWrite;
					memoryBarrier.dstAccessMask = accessMask::computeShader::shaderRead;

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
				VkPipelineStageFlags2 srcStage = pipelineStage::computeShader;
				VkPipelineStageFlags2 dstStage = pipelineStage::fragmentShader;
				VkAccessFlags2 srcAccessMask = accessMask::computeShader::shaderWrite;
				VkAccessFlags2 dstAccessMask = accessMask::fragmentShader::shaderRead;
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
		CommandPool& commandPool = GetCommandPool(Context::frameIndex, RenderStage::present);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		// Record present commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			// Viewport and scissor:
			VkViewport viewport = {};
			viewport.width = (float)Context::surface.GetCurrentExtent().width;
			viewport.height = (float)Context::surface.GetCurrentExtent().height;
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
			renderPassBeginInfo.renderArea.extent = Context::surface.GetCurrentExtent();

			// Begin render pass:
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			{
				// Update shader specific data:
				m_pPresentShaderProperties->SetTexture2d("renderTexture", RenderPassManager::GetForwardRenderPass()->GetRenderTexture());
				m_pPresentShaderProperties->UpdateShaderData();

				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_presentPipeline);

				vkCmdBindVertexBuffers(commandBuffer, 0, m_presentBindingCount, m_pPresentMaterial->GetMeshBuffers(m_pPresentMesh), m_pPresentMaterial->GetMeshOffsets(m_pPresentMesh));
				vkCmdBindIndexBuffer(commandBuffer, m_pPresentMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, Mesh::GetIndexType());

				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_presentPipelineLayout, 0, 1, &m_pPresentShaderProperties->GetDescriptorSet(Context::frameIndex), 0, nullptr);
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
		CommandPool& commandPool = GetCommandPool(Context::frameIndex, RenderStage::present);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		// Record present commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			// Render pass info:
			PresentRenderPass* presentRenderPass = RenderPassManager::GetPresentRenderPass();
			VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
			renderPassBeginInfo.renderPass = presentRenderPass->GetVkRenderPass();
			renderPassBeginInfo.framebuffer = presentRenderPass->GetFramebuffer(m_imageIndex);
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent = Context::surface.GetCurrentExtent();

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
		CommandPool& commandPool = GetCommandPool(Context::frameIndex, RenderStage::preRenderCompute);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();

		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &m_acquireSemaphores[Context::frameIndex];
		submitInfo.pWaitDstStageMask = &waitStage;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_preRenderComputeToShadowSemaphores[Context::frameIndex];

		VKA(vkQueueSubmit(Context::logicalDevice.GetGraphicsQueue().queue, 1, &submitInfo, nullptr));
	}
	void Renderer::SubmitShadowCommands()
	{
		PROFILE_FUNCTION();
		CommandPool& commandPool = GetCommandPool(Context::frameIndex, RenderStage::shadow);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();

		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &m_preRenderComputeToShadowSemaphores[Context::frameIndex];
		submitInfo.pWaitDstStageMask = &waitStage;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_shadowToForwardSemaphores[Context::frameIndex];

		VKA(vkQueueSubmit(Context::logicalDevice.GetGraphicsQueue().queue, 1, &submitInfo, nullptr));
	}
	void Renderer::SubmitForwardCommands()
	{
		PROFILE_FUNCTION();
		CommandPool& commandPool = GetCommandPool(Context::frameIndex, RenderStage::forward);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		
		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &m_shadowToForwardSemaphores[Context::frameIndex];
		submitInfo.pWaitDstStageMask = &waitStage;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_forwardToPostRenderComputeSemaphores[Context::frameIndex];

		VKA(vkQueueSubmit(Context::logicalDevice.GetGraphicsQueue().queue, 1, &submitInfo, nullptr));
	}
	void Renderer::SubmitForwardCommandsParallel()
	{
		PROFILE_FUNCTION();
		CommandPool& commandPool = GetCommandPool(Context::frameIndex, RenderStage::forward);
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
				memoryBarrier.srcStageMask = pipelineStage::vertexShader;
				memoryBarrier.dstStageMask = pipelineStage::computeShader;
				memoryBarrier.srcAccessMask = accessMask::vertexShader::shaderRead;
				memoryBarrier.dstAccessMask = accessMask::computeShader::shaderWrite;

				VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
				dependencyInfo.memoryBarrierCount = 1;
				dependencyInfo.pMemoryBarriers = &memoryBarrier;

				vkCmdPipelineBarrier2(primaryCommandBuffer, &dependencyInfo);
			}
		}
		vkEndCommandBuffer(primaryCommandBuffer);

		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &m_shadowToForwardSemaphores[Context::frameIndex];
		submitInfo.pWaitDstStageMask = &waitStage;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &primaryCommandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_forwardToPostRenderComputeSemaphores[Context::frameIndex];

		VKA(vkQueueSubmit(Context::logicalDevice.GetGraphicsQueue().queue, 1, &submitInfo, nullptr));
	}
	void Renderer::SubmitPostRenderComputeCommands()
	{
		PROFILE_FUNCTION();
		CommandPool& commandPool = GetCommandPool(Context::frameIndex, RenderStage::postRenderCompute);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();

		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &m_forwardToPostRenderComputeSemaphores[Context::frameIndex];
		submitInfo.pWaitDstStageMask = &waitStage;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_postRenderToPresentSemaphores[Context::frameIndex];

		VKA(vkQueueSubmit(Context::logicalDevice.GetGraphicsQueue().queue, 1, &submitInfo, nullptr));
	}
	void Renderer::SubmitPresentCommands()
	{
		PROFILE_FUNCTION();
		CommandPool& commandPool = GetCommandPool(Context::frameIndex, RenderStage::present);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();

		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &m_postRenderToPresentSemaphores[Context::frameIndex];
		submitInfo.pWaitDstStageMask = &waitStage;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_releaseSemaphores[Context::frameIndex];

		VKA(vkQueueSubmit(Context::logicalDevice.GetGraphicsQueue().queue, 1, &submitInfo, m_frameFences[Context::frameIndex]));
	}

	bool Renderer::PresentImage()
	{
		PROFILE_FUNCTION();
		VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_releaseSemaphores[Context::frameIndex];
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &Context::GetVkSwapchainKHR();
		presentInfo.pImageIndices = &m_imageIndex;

		VkResult result = vkQueuePresentKHR(Context::logicalDevice.GetPresentQueue().queue, &presentInfo);

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

	void Renderer::CreateFences()
	{
		VkFenceCreateInfo createInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
		createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;	// Fence is created in the signaled state to prevent the first wait from blocking.

		m_frameFences.resize(Context::framesInFlight);
		for (uint32_t i = 0; i < Context::framesInFlight; i++)
		{
			VKA(vkCreateFence(Context::GetVkDevice(), &createInfo, nullptr, &m_frameFences[i]));
			NAME_VK_FENCE(m_frameFences[i], "FrameFences" + std::to_string(i));
		}
	}
	void Renderer::CreateSemaphores()
	{
		VkSemaphoreCreateInfo createInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

        // One per frame in flight:
		m_acquireSemaphores.resize(Context::framesInFlight);
		m_preRenderComputeToShadowSemaphores.resize(Context::framesInFlight);
		m_shadowToForwardSemaphores.resize(Context::framesInFlight);
		m_forwardToPostRenderComputeSemaphores.resize(Context::framesInFlight);
		m_postRenderToPresentSemaphores.resize(Context::framesInFlight);
		m_releaseSemaphores.resize(Context::framesInFlight);
		for (uint32_t i = 0; i < Context::framesInFlight; i++)
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
		for (uint32_t i = 0; i < Context::framesInFlight; i++)
			vkDestroyFence(Context::GetVkDevice(), m_frameFences[i], nullptr);
		m_frameFences.clear();
	}
	void Renderer::DestroySemaphores()
	{
		for (uint32_t i = 0; i < Context::framesInFlight; i++)
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
		assert(renderStage < (int)RenderStage::stageCount);
		assert(frameIndex < Context::framesInFlight);
		return m_commandPools[frameIndex + (int)renderStage * Context::framesInFlight];
	}
}