#include "immediateCompute.h"
#include "computeShader.h"
#include "logger.h"
#include "shaderProperties.h"
#include "vulkanComputePushConstant.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include "vulkanPipeline.h"
#include "vulkanSingleTimeCommand.h"
#include <iostream>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Static members:
	bool Immediate::s_isInitialized = false;



	// Initialization/Cleanup:
	void Immediate::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;
	}
	void Immediate::Clear()
	{

	}



	// Immediate dispatch call:
	void Immediate::Dispatch(ComputeShader* pComputeShader, ShaderProperties* pShaderProperties, Uint3 threadCount, float time, float deltaTime)
	{
		if (!pComputeShader)
		{
			LOG_ERROR("compute::Immediately::Dispatch(...) failed. pComputeShader is nullptr.");
			return;
		}
		if (!pShaderProperties)
		{
			LOG_ERROR("compute::Immediately::Dispatch(...) failed. pShaderProperties is nullptr.");
			return;
		}
		if (threadCount[0] == 0 || threadCount[1] == 0 || threadCount[2] == 0)
		{
			LOG_ERROR("compute::Immediately::Dispatch(...) failed. threadCount has 0 entry.");
			return;
		}

		// Update shader specific data:
		pShaderProperties->UpdateShaderData(0);

		// Record command buffer:
		VkCommandBuffer commandBuffer = SingleTimeCommand::BeginCommand(Context::logicalDevice.GetComputeQueue());
		{
			// Set pipeline:
			VkPipeline pipeline = pComputeShader->GetPipeline()->GetVkPipeline();
			VkPipelineLayout pipelineLayout = pComputeShader->GetPipeline()->GetVkPipelineLayout();
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
			ComputePushConstant pushConstant(threadCount, time, deltaTime);
			vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstant), &pushConstant);

			// Compute group count:
			Uint3 blockSize = pComputeShader->GetBlockSize();
			uint32_t groupCountX = (threadCount.x + blockSize.x - 1) / blockSize.x;
			uint32_t groupCountY = (threadCount.y + blockSize.y - 1) / blockSize.y;
			uint32_t groupCountZ = (threadCount.z + blockSize.z - 1) / blockSize.z;

			// Dispatch compute shader:
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &pShaderProperties->GetDescriptorSet(0), 0, nullptr);
			vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
		}
		SingleTimeCommand::EndCommand(Context::logicalDevice.GetComputeQueue());
	}
}