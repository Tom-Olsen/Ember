#include "pipeline.h"
#include "vulkanMacros.h"
#include "vulkanPushConstant.h"



// Destructor:
Pipeline::~Pipeline()
{
	vkDestroyDescriptorSetLayout(context->LogicalDevice(), descriptorSetLayout, nullptr);
	vkDestroyPipelineLayout(context->LogicalDevice(), pipelineLayout, nullptr);
	vkDestroyPipeline(context->LogicalDevice(), pipeline, nullptr);
}



// Protected methods:
void Pipeline::CreatePipelineLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings, VkShaderStageFlags flags)
{
    // Descriptor set layout:
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
    descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    descriptorSetLayoutCreateInfo.pBindings = bindings.data();
    VKA(vkCreateDescriptorSetLayout(context->LogicalDevice(), &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout));

    // Push constants layout:
    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = flags;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(VulkanPushConstant);

    // Pipeline layout:
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
    vkCreatePipelineLayout(context->LogicalDevice(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
}
VkShaderModule Pipeline::CreateShaderModule(const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
    createInfo.codeSize = code.size();
    createInfo.pCode = (uint32_t*)(code.data());

    VkShaderModule shaderModule;
    VKA(vkCreateShaderModule(context->LogicalDevice(), &createInfo, nullptr, &shaderModule));
    return shaderModule;
}