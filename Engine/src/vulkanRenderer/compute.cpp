#include "compute.h"
#include "computePushConstant.h"
#include "computeShader.h"
#include "emberTime.h"
#include "logger.h"
#include "pipeline.h"
#include "shaderProperties.h"
#include "vulkanCommand.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include "vulkanUtility.h"
#include <vulkan/vulkan.h>



namespace emberEngine
{
	// Static members:
	bool Compute::s_isInitialized = false;
	uint32_t Compute::s_callIndex = 0;
	std::vector<ComputeCall> Compute::s_staticComputeCalls;
	std::vector<ComputeCall> Compute::s_dynamicComputeCalls;
	std::vector<ComputeCall> Compute::s_asyncComputeCalls;
	std::vector<ComputeCall*> Compute::s_computeCallPointers;
	std::unordered_map<ComputeShader*, ResourcePool<ShaderProperties, 10>> Compute::s_shaderPropertiesPoolMap;
	std::unordered_map<ComputeShader*, ResourcePool<ShaderProperties, 10>> Compute::s_asyncShaderPropertiesPoolMap;
	VulkanCommand* Compute::s_pImmediatCommand;
	VulkanCommand* Compute::s_pAsyncCommand;
	VkFence Compute::s_immediatFence;
	VkFence Compute::s_asyncFence;



	// Initialization/Cleanup:
	void Compute::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;
		s_pImmediatCommand = new VulkanCommand(VulkanContext::pLogicalDevice->GetComputeQueue());
		s_pAsyncCommand = new VulkanCommand(VulkanContext::pLogicalDevice->GetComputeQueue());
		VkFenceCreateInfo createInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
		VKA(vkCreateFence(VulkanContext::GetVkDevice(), &createInfo, nullptr, &s_immediatFence));
		VKA(vkCreateFence(VulkanContext::GetVkDevice(), &createInfo, nullptr, &s_asyncFence));
	}
	void Compute::Clear()
	{
		ResetComputeCalls();
		s_shaderPropertiesPoolMap.clear();
		s_asyncShaderPropertiesPoolMap.clear();
		vkDestroyFence(VulkanContext::GetVkDevice(), s_immediatFence, nullptr);
		vkDestroyFence(VulkanContext::GetVkDevice(), s_asyncFence, nullptr);
	}



	// Public methods:
	// Graphics synced dispatch calls:
	ShaderProperties* Compute::Dispatch(ComputeShader* pComputeShader, uint32_t threadCountX, uint32_t threadCountY, uint32_t threadCountZ)
	{
		return Dispatch(pComputeShader, Uint3(threadCountX, threadCountY, threadCountZ));
	}
	ShaderProperties* Compute::Dispatch(ComputeShader* pComputeShader, Uint3 threadCount)
	{
		if (!pComputeShader)
		{
			LOG_ERROR("Compute::Dispatch(...) failed. pComputeShader is nullptr.");
			return nullptr;
		}

		// Setup compute call:
		threadCount = Uint3::Max(Uint3::one, threadCount);
		ShaderProperties* pShaderProperties = s_shaderPropertiesPoolMap[pComputeShader].Acquire((Shader*)pComputeShader);
		ComputeCall computeCall = { s_callIndex, threadCount, pComputeShader, pShaderProperties, VK_ACCESS_2_NONE, VK_ACCESS_2_NONE };
		s_dynamicComputeCalls.push_back(computeCall);
		s_callIndex++;

		// By returning pShaderProperties, we allow user to change the shader properties of the compute call:
		return pShaderProperties;
	}
	void Compute::Dispatch(ComputeShader* pComputeShader, ShaderProperties* pShaderProperties, uint32_t threadCountX, uint32_t threadCountY, uint32_t threadCountZ)
	{
		Dispatch(pComputeShader, pShaderProperties, Uint3(threadCountX, threadCountY, threadCountZ));
	}
	void Compute::Dispatch(ComputeShader* pComputeShader, ShaderProperties* pShaderProperties, Uint3 threadCount)
	{
		if (!pComputeShader)
		{
			LOG_ERROR("Compute::Dispatch(...) failed. pComputeShader is nullptr.");
			return;
		}
		if (!pShaderProperties)
		{
			LOG_ERROR("Compute::Dispatch(...) failed. pShaderProperties is nullptr.");
			return;
		}

		// Setup compute call:
		threadCount = Uint3::Max(Uint3::one, threadCount);
		ComputeCall computeCall = { s_callIndex, threadCount, pComputeShader, pShaderProperties, VK_ACCESS_2_NONE, VK_ACCESS_2_NONE };
		s_staticComputeCalls.push_back(computeCall);
		s_callIndex++;
	}

	// Async dispatch calls:
	void Compute::DispatchImmediatly(ComputeShader* pComputeShader, ShaderProperties* pShaderProperties, Uint3 threadCount)
	{
		if (!pComputeShader)
		{
			LOG_ERROR("Compute::DispatchImmediatly(...) failed. pComputeShader is nullptr.");
			return;
		}
		if (!pShaderProperties)
		{
			LOG_ERROR("Compute::DispatchImmediatly(...) failed. pShaderProperties is nullptr.");
			return;
		}

		// Reset comand pool:
		vkResetCommandPool(VulkanContext::GetVkDevice(), s_pImmediatCommand->GetVkCommandPool(), 0);
		VkCommandBuffer commandBuffer = s_pImmediatCommand->GetVkCommandBuffer();

		// Update shader specific data:
		uint32_t frameIndex = 0;	// immediate calls don't need multiple descriptorSets. Thus use 0th descriptorSet by default.
		threadCount = Uint3::Max(Uint3::one, threadCount);
		pShaderProperties->UpdateShaderData(frameIndex);

		// Record command buffer:
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			// Set pipeline:
			VkPipeline pipeline = pComputeShader->GetPipeline()->GetVkPipeline();
			VkPipelineLayout pipelineLayout = pComputeShader->GetPipeline()->GetVkPipelineLayout();
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
			ComputePushConstant pushConstant(threadCount, Time::GetTime(), Time::GetDeltaTime());
			vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstant), &pushConstant);

			// Compute group count:
			Uint3 blockSize = pComputeShader->GetBlockSize();
			uint32_t groupCountX = (threadCount.x + blockSize.x - 1) / blockSize.x;
			uint32_t groupCountY = (threadCount.y + blockSize.y - 1) / blockSize.y;
			uint32_t groupCountZ = (threadCount.z + blockSize.z - 1) / blockSize.z;

			// Dispatch compute shader:
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &pShaderProperties->GetDescriptorSet(frameIndex), 0, nullptr);
			vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
		}
		VKA(vkEndCommandBuffer(commandBuffer));
		
		// Submit command buffer:
		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitSemaphores = VK_NULL_HANDLE;
		submitInfo.pWaitDstStageMask = nullptr;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;
		VKA(vkResetFences(VulkanContext::GetVkDevice(), 1, &s_immediatFence));
		VKA(vkQueueSubmit(VulkanContext::pLogicalDevice->GetComputeQueue().queue, 1, &submitInfo, s_immediatFence));
		VKA(vkWaitForFences(VulkanContext::GetVkDevice(), 1, &s_immediatFence, VK_TRUE, UINT64_MAX));
	}
	void Compute::BeginAsyncCompute()
	{
		// Reset comand pool:
		vkResetCommandPool(VulkanContext::GetVkDevice(), s_pAsyncCommand->GetVkCommandPool(), 0);

		// Begin command buffer recording:
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VKA(vkBeginCommandBuffer(s_pAsyncCommand->GetVkCommandBuffer(), &beginInfo));
	}
	ShaderProperties* Compute::DispatchAsync(ComputeShader* pComputeShader, Uint3 threadCount)
	{
		if (!pComputeShader)
		{
			LOG_ERROR("Compute::DispatchAsync(...) failed. pComputeShader is nullptr.");
			return nullptr;
		}

		VkCommandBuffer commandBuffer = s_pAsyncCommand->GetVkCommandBuffer();

		// Update shader specific data:
		uint32_t frameIndex = 0;	// async calls don't need multiple descriptorSets. Thus use 0th descriptorSet by default.
		threadCount = Uint3::Max(Uint3::one, threadCount);
		ShaderProperties* pShaderProperties = s_asyncShaderPropertiesPoolMap[pComputeShader].Acquire((Shader*)pComputeShader);

		// Set pipeline:
		VkPipeline pipeline = pComputeShader->GetPipeline()->GetVkPipeline();
		VkPipelineLayout pipelineLayout = pComputeShader->GetPipeline()->GetVkPipelineLayout();
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
		ComputePushConstant pushConstant(threadCount, Time::GetTime(), Time::GetDeltaTime());
		vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstant), &pushConstant);

		// Compute group count:
		Uint3 blockSize = pComputeShader->GetBlockSize();
		uint32_t groupCountX = (threadCount.x + blockSize.x - 1) / blockSize.x;
		uint32_t groupCountY = (threadCount.y + blockSize.y - 1) / blockSize.y;
		uint32_t groupCountZ = (threadCount.z + blockSize.z - 1) / blockSize.z;

		// Dispatch compute shader:
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &pShaderProperties->GetDescriptorSet(frameIndex), 0, nullptr);
		vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);

		// Record dispach as computeCall for later shaderProperties cleanup:
		ComputeCall computeCall = { 0, threadCount, pComputeShader, pShaderProperties, VK_ACCESS_2_NONE, VK_ACCESS_2_NONE };
		s_asyncComputeCalls.push_back(computeCall);

		return pShaderProperties;
	}
	void Compute::DispatchAsync(ComputeShader* pComputeShader, ShaderProperties* pShaderProperties, Uint3 threadCount)
	{
		if (!pComputeShader)
		{
			LOG_ERROR("Compute::DispatchAsync(...) failed. pComputeShader is nullptr.");
			return;
		}
		if (!pShaderProperties)
		{
			LOG_ERROR("Compute::DispatchAsync(...) failed. pShaderProperties is nullptr.");
			return;
		}

		VkCommandBuffer commandBuffer = s_pAsyncCommand->GetVkCommandBuffer();

		// Update shader specific data:
		uint32_t frameIndex = 0;	// async calls don't need multiple descriptorSets. Thus use 0th descriptorSet by default.
		threadCount = Uint3::Max(Uint3::one, threadCount);
		pShaderProperties->UpdateShaderData(frameIndex);

		// Set pipeline:
		VkPipeline pipeline = pComputeShader->GetPipeline()->GetVkPipeline();
		VkPipelineLayout pipelineLayout = pComputeShader->GetPipeline()->GetVkPipelineLayout();
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
		ComputePushConstant pushConstant(threadCount, Time::GetTime(), Time::GetDeltaTime());
		vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstant), &pushConstant);

		// Compute group count:
		Uint3 blockSize = pComputeShader->GetBlockSize();
		uint32_t groupCountX = (threadCount.x + blockSize.x - 1) / blockSize.x;
		uint32_t groupCountY = (threadCount.y + blockSize.y - 1) / blockSize.y;
		uint32_t groupCountZ = (threadCount.z + blockSize.z - 1) / blockSize.z;

		// Dispatch compute shader:
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &pShaderProperties->GetDescriptorSet(frameIndex), 0, nullptr);
		vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
	}
	void Compute::EndAsyncCompute()
	{
		// Update shader specific data of compute calls wit dynamical shader properties:
		uint32_t frameIndex = 0;	// async calls don't need multiple descriptorSets. Thus use 0th descriptorSet by default.
		for (ComputeCall& computeCall : s_asyncComputeCalls)
			computeCall.pShaderProperties->UpdateShaderData(frameIndex);

		VkCommandBuffer commandBuffer = s_pAsyncCommand->GetVkCommandBuffer();

		// End command buffer recording:
		VKA(vkEndCommandBuffer(commandBuffer));

		// Submit command buffer:
		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitSemaphores = VK_NULL_HANDLE;
		submitInfo.pWaitDstStageMask = nullptr;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;
		VKA(vkResetFences(VulkanContext::GetVkDevice(), 1, &s_asyncFence));
		VKA(vkQueueSubmit(VulkanContext::pLogicalDevice->GetComputeQueue().queue, 1, &submitInfo, s_asyncFence));
		VKA(vkWaitForFences(VulkanContext::GetVkDevice(), 1, &s_asyncFence, VK_TRUE, UINT64_MAX));

		// Return all pShaderProperties of async compute calls back to the corresponding pool:
		for (ComputeCall& computeCall : s_asyncComputeCalls)
			s_asyncShaderPropertiesPoolMap[computeCall.pComputeShader].Release(computeCall.pShaderProperties);
		
		// Shrink all async pools back to max number of async computeCalls of last begin/end block:
		for (auto& [_, pool] : s_asyncShaderPropertiesPoolMap)
			pool.ShrinkToFit();
	}

	// Barriers:
	void Compute::Barrier(VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask)
	{
		ComputeCall computeCall = { s_callIndex, Uint3::zero, nullptr, nullptr, srcAccessMask, dstAccessMask };
		s_staticComputeCalls.push_back(computeCall);
		s_callIndex++;
	}
	void Compute::BarrierAsync(VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask)
	{
		VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
		memoryBarrier.srcStageMask = PipelineStage::computeShader;
		memoryBarrier.dstStageMask = PipelineStage::computeShader;
		memoryBarrier.srcAccessMask = srcAccessMask;
		memoryBarrier.dstAccessMask = dstAccessMask;

		VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
		dependencyInfo.memoryBarrierCount = 1;
		dependencyInfo.pMemoryBarriers = &memoryBarrier;

		vkCmdPipelineBarrier2(s_pAsyncCommand->GetVkCommandBuffer(), &dependencyInfo);
	}



	// Management:
	std::vector<ComputeCall*>& Compute::GetComputeCallPointers()
	{
		// Populate draw call pointers vector according to callIndex:
		s_computeCallPointers.clear();
		s_computeCallPointers.resize(s_staticComputeCalls.size() + s_dynamicComputeCalls.size());
		for (auto& computeCall : s_staticComputeCalls)
			s_computeCallPointers[computeCall.callIndex] = &computeCall;
		for (auto& computeCall : s_dynamicComputeCalls)
			s_computeCallPointers[computeCall.callIndex] = &computeCall;

		return s_computeCallPointers;
	}
	void Compute::ResetComputeCalls()
	{
		// Return all pShaderProperties of compute calls back to the corresponding pool:
		for (ComputeCall& computeCall : s_dynamicComputeCalls)
			s_shaderPropertiesPoolMap[computeCall.pComputeShader].Release(computeCall.pShaderProperties);

		// Shrink all pools back to max number of computeCalls of last frame:
		for (auto& [_, pool] : s_shaderPropertiesPoolMap)
			pool.ShrinkToFit();

		// Remove all computeCalls so next frame can start fresh:
		s_staticComputeCalls.clear();
		s_dynamicComputeCalls.clear();
		s_callIndex = 0;
	}
	void Compute::PrintComputeCalls()
	{
		for (int i = 0; i < s_computeCallPointers.size(); i++)
		{
			if (s_computeCallPointers[i] == nullptr)
				break;

			LOG_TRACE(s_computeCallPointers[i]->ToString());
		}
	}
}