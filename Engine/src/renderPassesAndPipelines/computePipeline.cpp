#include "computePipeline.h"
#include "computePushConstant.h"
#include "spirvReflect.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"



namespace emberEngine
{
    // Constructor/Destructor:
    ComputePipeline::ComputePipeline(const std::vector<char>& computeCode, const std::vector<VkDescriptorSetLayoutBinding>& vkDescriptorSetLayoutBindings)
    {
        // Create pipeline Layout:
        CreatePipelineLayout(vkDescriptorSetLayoutBindings);

        // Create compute shader module from .spv files:
        VkShaderModule computeShaderModule = CreateShaderModule(computeCode);

        // Create pipeline:
        CreatePipeline(computeShaderModule);

        // Destroy shader module (only needed for pipeline creation):
        vkDestroyShaderModule(VulkanContext::GetVkDevice(), computeShaderModule, nullptr);
    }
    ComputePipeline::~ComputePipeline()
    {

    }



    // Private:
    void ComputePipeline::CreatePipelineLayout(const std::vector<VkDescriptorSetLayoutBinding>& vkDescriptorSetLayoutBindings)
    {
        // Descriptor set layout:
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
        descriptorSetLayoutCreateInfo.bindingCount = vkDescriptorSetLayoutBindings.size();
        descriptorSetLayoutCreateInfo.pBindings = vkDescriptorSetLayoutBindings.data();
        VKA(vkCreateDescriptorSetLayout(VulkanContext::GetVkDevice(), &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayout));
        
        // Push constants layout:
        VkPushConstantRange pushConstantRange = {};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(ComputePushConstant);
    
        // Pipeline layout:
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutCreateInfo.setLayoutCount = 1;
        pipelineLayoutCreateInfo.pSetLayouts = &m_descriptorSetLayout;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
        pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
        vkCreatePipelineLayout(VulkanContext::GetVkDevice(), &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout);
    }
    void ComputePipeline::CreatePipeline(const VkShaderModule& computeShaderModule)
    {
        VkPipelineShaderStageCreateInfo computeShaderStageCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
        computeShaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        computeShaderStageCreateInfo.module = computeShaderModule;
        computeShaderStageCreateInfo.pName = "main";

        VkComputePipelineCreateInfo pipelineInfo = {VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO};
        pipelineInfo.stage = computeShaderStageCreateInfo;
        pipelineInfo.layout = m_pipelineLayout;
        
        VKA(vkCreateComputePipelines(VulkanContext::GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline))
    }
}