#include "vulkanImmediateCompute.h"
#include "logger.h"
#include "vulkanComputePushConstant.h"
#include "vulkanComputeShader.h"
#include "vulkanContext.h"
#include "vulkanLogicalDevice.h"
#include "vulkanPipeline.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanRenderer.h"
#include "vulkanSingleTimeCommand.h"
#include <iostream>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	Immediate::Immediate()
	{

	}
	Immediate::~Immediate()
	{

	}



	// Movable:
	Immediate::Immediate(Immediate&& other) noexcept = default;
	Immediate& Immediate::operator=(Immediate&& other) noexcept = default;



	// Immediate dispatch call:
	void Immediate::Dispatch(emberBackendInterface::IComputeShader* pIComputeShader, emberBackendInterface::IDescriptorSetBinding* pIDescriptorSetBinding, Uint3 threadCount, float time, float deltaTime)
	{
		if (!pIComputeShader)
		{
			LOG_ERROR("compute::Immediately::Dispatch(...) failed. pIComputeShader is nullptr.");
			return;
		}
		if (!pIDescriptorSetBinding)
		{
			LOG_ERROR("compute::Immediately::Dispatch(...) failed. pIDescriptorSetBinding is nullptr.");
			return;
		}
		if (threadCount[0] == 0 || threadCount[1] == 0 || threadCount[2] == 0)
		{
			LOG_ERROR("compute::Immediately::Dispatch(...) failed. threadCount has 0 entry.");
			return;
		}

		// Update shader specific data:
		DescriptorSetBinding* pDescriptorSetBinding = static_cast<DescriptorSetBinding*>(pIDescriptorSetBinding);
		pDescriptorSetBinding->UpdateShaderData(0);

		// Record command buffer:
		VkCommandBuffer commandBuffer = SingleTimeCommand::BeginCommand(Context::GetLogicalDevice()->GetComputeQueue());
		{
			// Set pipeline:
			ComputeShader* pComputeShader = static_cast<ComputeShader*>(pIComputeShader);
			VkPipeline pipeline = pComputeShader->GetPipeline()->GetVkPipeline();
			VkPipelineLayout pipelineLayout = pComputeShader->GetVkPipelineLayout();
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
			ComputePushConstant pushConstant(threadCount, time, deltaTime);
			vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstant), &pushConstant);

			// Compute group count:
			Uint3 blockSize = pComputeShader->GetBlockSize();
			uint32_t groupCountX = (threadCount.x + blockSize.x - 1) / blockSize.x;
			uint32_t groupCountY = (threadCount.y + blockSize.y - 1) / blockSize.y;
			uint32_t groupCountZ = (threadCount.z + blockSize.z - 1) / blockSize.z;

			// Dispatch compute shader:
			uint32_t frameIndex = Context::GetFrameIndex();
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 3, Context::GetRenderer()->GetStaticDescriptorSets(frameIndex).data(), 0, nullptr);
			if (VkDescriptorSet vkDescriptorSet = pComputeShader->GetDescriptorSetBinding()->GetVkDescriptorSet(frameIndex); vkDescriptorSet != VK_NULL_HANDLE)
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, SHADER_SET_INDEX, 1, &vkDescriptorSet, 0, nullptr);
			vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
		}
		SingleTimeCommand::EndCommand(Context::GetLogicalDevice()->GetComputeQueue());
	}
}