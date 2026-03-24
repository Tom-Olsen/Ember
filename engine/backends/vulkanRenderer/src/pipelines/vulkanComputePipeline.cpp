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
        m_name = name;

        // Create compute shader module from .spv files:
        VkShaderModule computeShaderModule = CreateShaderModule(computeCode);

        // Create pipeline:
        CreatePipeline(vkPipelineLayout, computeShaderModule);

        // Destroy shader module (only needed for pipeline creation):
        vkDestroyShaderModule(Context::GetVkDevice(), computeShaderModule, nullptr);
        NAME_VK_OBJECT(m_pipeline, m_name + "ComputePipeline");
    }
    ComputePipeline::~ComputePipeline()
    {

    }



    // Private:
    //void ComputePipeline::CreatePipelineLayout(VkPipelineLayout vkPipelineLayout, const VkShaderModule& computeShaderModule)
    //{
    //    // Push constants layout:
    //    VkPushConstantRange pushConstantRange = {};
    //    pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    //    pushConstantRange.offset = 0;
    //    pushConstantRange.size = sizeof(ComputePushConstant);
    //
    //    // Pipeline layout:
    //    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
    //    pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(vkDescriptorSetLayouts.size());
    //    pipelineLayoutCreateInfo.pSetLayouts = vkDescriptorSetLayouts.data();
    //    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    //    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
    //    vkCreatePipelineLayout(Context::GetVkDevice(), &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout);
    //    NAME_VK_OBJECT(m_pipelineLayout, m_name + "ComputePipelineLayout");
    //}
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