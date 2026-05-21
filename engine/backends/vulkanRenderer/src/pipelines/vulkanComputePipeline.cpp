#include "vulkanComputePipeline.h"
#include "vulkanComputePushConstant.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
    // Constructor/Destructor:
    ComputePipeline::ComputePipeline(const std::string& name, VkPipelineLayout vkPipelineLayout, const std::vector<char>& computeCode)
    {
        // Create compute shader module from .spv files:
        VkShaderModule computeShaderModule = CreateShaderModule(computeCode, "ShaderModule_Compute_" + name);

        // Create pipeline:
        CreatePipeline(vkPipelineLayout, computeShaderModule);

        // Destroy shader module (only needed for pipeline creation):
        vkDestroyShaderModule(Context::GetVkDevice(), computeShaderModule, nullptr);
        NAME_VK_OBJECT(m_pipeline, "Pipeline_Compute_" + name);
    }
    ComputePipeline::~ComputePipeline()
    {

    }



    // Private:
    void ComputePipeline::CreatePipeline(VkPipelineLayout vkPipelineLayout, const VkShaderModule& computeShaderModule)
    {
        VkPipelineShaderStageCreateInfo computeShaderStageCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
        computeShaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        computeShaderStageCreateInfo.module = computeShaderModule;
        computeShaderStageCreateInfo.pName = "main";

        VkComputePipelineCreateInfo pipelineInfo = { VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO };
        pipelineInfo.stage = computeShaderStageCreateInfo;
        pipelineInfo.layout = vkPipelineLayout;

        VKA(vkCreateComputePipelines(Context::GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline))
    }
}