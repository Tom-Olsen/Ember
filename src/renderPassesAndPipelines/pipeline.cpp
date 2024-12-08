#include "pipeline.h"
#include "vulkanMacros.h"



// Destructor:
Pipeline::~Pipeline()
{
	vkDestroyDescriptorSetLayout(context->GetVkDevice(), descriptorSetLayout, nullptr);
	vkDestroyPipelineLayout(context->GetVkDevice(), pipelineLayout, nullptr);
	vkDestroyPipeline(context->GetVkDevice(), pipeline, nullptr);
}



// Protected methods:
VkShaderModule Pipeline::CreateShaderModule(const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
    createInfo.codeSize = code.size();
    createInfo.pCode = (uint32_t*)(code.data());

    VkShaderModule shaderModule;
    VKA(vkCreateShaderModule(context->GetVkDevice(), &createInfo, nullptr, &shaderModule));
    return shaderModule;
}