#include "vulkanImmediateCompute.h"
#include "logger.h"
#include "vulkanComputePushConstant.h"
#include "vulkanComputeShader.h"
#include "vulkanContext.h"
#include "vulkanLogicalDevice.h"
#include "vulkanPipeline.h"
#include "vulkanShaderProperties.h"
#include "vulkanSingleTimeCommand.h"
#include <iostream>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Constructor/Destructor:
	Immediate::Immediate()
	{

	}
	Immediate::~Immediate()
	{

	}



	// Immediate dispatch call:
	void Immediate::Dispatch(emberBackendInterface::IComputeShader* pIComputeShader, emberBackendInterface::IShaderProperties* pIShaderProperties, Uint3 threadCount, float time, float deltaTime)
	{
		if (!pIComputeShader)
		{
			LOG_ERROR("compute::Immediately::Dispatch(...) failed. pIComputeShader is nullptr.");
			return;
		}
		if (!pIShaderProperties)
		{
			LOG_ERROR("compute::Immediately::Dispatch(...) failed. pIShaderProperties is nullptr.");
			return;
		}
		if (threadCount[0] == 0 || threadCount[1] == 0 || threadCount[2] == 0)
		{
			LOG_ERROR("compute::Immediately::Dispatch(...) failed. threadCount has 0 entry.");
			return;
		}

		// Update shader specific data:
		ShaderProperties* pShaderProps = static_cast<ShaderProperties*>(pIShaderProperties);
		pShaderProps->UpdateShaderData(0);

		// Record command buffer:
		VkCommandBuffer commandBuffer = SingleTimeCommand::BeginCommand(Context::GetLogicalDevice()->GetComputeQueue());
		{
			// Set pipeline:
			ComputeShader* pCompShader = static_cast<ComputeShader*>(pIComputeShader);
			VkPipeline pipeline = pCompShader->GetPipeline()->GetVkPipeline();
			VkPipelineLayout pipelineLayout = pCompShader->GetPipeline()->GetVkPipelineLayout();
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
			ComputePushConstant pushConstant(threadCount, time, deltaTime);
			vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstant), &pushConstant);

			// Compute group count:
			Uint3 blockSize = pCompShader->GetBlockSize();
			uint32_t groupCountX = (threadCount.x + blockSize.x - 1) / blockSize.x;
			uint32_t groupCountY = (threadCount.y + blockSize.y - 1) / blockSize.y;
			uint32_t groupCountZ = (threadCount.z + blockSize.z - 1) / blockSize.z;

			// Dispatch compute shader:
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &pShaderProps->GetDescriptorSet(0), 0, nullptr);
			vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
		}
		SingleTimeCommand::EndCommand(Context::GetLogicalDevice()->GetComputeQueue());
	}
}