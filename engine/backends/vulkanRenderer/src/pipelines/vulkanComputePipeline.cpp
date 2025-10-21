#include "vulkanComputePipeline.h"
#include "spirvReflect.h"
#include "vulkanComputePushConstant.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
    // Constructor/Destructor:
    ComputePipeline::ComputePipeline(const std::string& name, const std::vector<char>& computeCode, std::vector<DescriptorSetLayoutBinding>& descriptorSetLayoutBindings)
    {
        m_name = name;

        // Create pipeline Layout:
        CreatePipelineLayout(descriptorSetLayoutBindings);

        // Create compute shader module from .spv files:
        VkShaderModule computeShaderModule = CreateShaderModule(computeCode);

        // Create pipeline:
        CreatePipeline(computeShaderModule);

        // Destroy shader module (only needed for pipeline creation):
        vkDestroyShaderModule(Context::GetVkDevice(), computeShaderModule, nullptr);
        NAME_VK_PIPELINE(m_pipeline, m_name + "ComputePipeline");
    }
    ComputePipeline::~ComputePipeline()
    {

    }



    // Private:
    void ComputePipeline::CreatePipelineLayout(std::vector<DescriptorSetLayoutBinding>& descriptorSetLayoutBindings)
    {
        // Descriptor set layout:
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
        descriptorSetLayoutCreateInfo.bindingCount = descriptorSetLayoutBindings.size();
        descriptorSetLayoutCreateInfo.pBindings = reinterpret_cast<VkDescriptorSetLayoutBinding*>(descriptorSetLayoutBindings.data());
        VKA(vkCreateDescriptorSetLayout(Context::GetVkDevice(), &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayout));

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
        vkCreatePipelineLayout(Context::GetVkDevice(), &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout);
        NAME_VK_PIPELINE_LAYOUT(m_pipelineLayout, m_name + "ComputePipelineLayout");
    }
    void ComputePipeline::CreatePipeline(const VkShaderModule& computeShaderModule)
    {
        VkPipelineShaderStageCreateInfo computeShaderStageCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
        computeShaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        computeShaderStageCreateInfo.module = computeShaderModule;
        computeShaderStageCreateInfo.pName = "main";

        VkComputePipelineCreateInfo pipelineInfo = { VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO };
        pipelineInfo.stage = computeShaderStageCreateInfo;
        pipelineInfo.layout = m_pipelineLayout;

        VKA(vkCreateComputePipelines(Context::GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline))
    }
}