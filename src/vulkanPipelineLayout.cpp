#include "vulkanPipelineLayout.h"
#include "macros.h"



// Constructor:
VulkanPipelineLayout::VulkanPipelineLayout(VulkanLogicalDevice* logicalDevice)
{
    this->logicalDevice = logicalDevice;

    VkDescriptorSetLayoutBinding descriptorSetlayoutBinding = {};
    descriptorSetlayoutBinding.binding = 0;                             // binding point in shader
    descriptorSetlayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorSetlayoutBinding.descriptorCount = 1;                     // allows for arrays of descriptors
    descriptorSetlayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; // used in vertex shader
    descriptorSetlayoutBinding.pImmutableSamplers = nullptr;            // needed later for image sampling

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
    descriptorSetLayoutCreateInfo.bindingCount = 1;
    descriptorSetLayoutCreateInfo.pBindings = &descriptorSetlayoutBinding;

    VKA(vkCreateDescriptorSetLayout(logicalDevice->device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout));

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
    vkCreatePipelineLayout(logicalDevice->device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
}



// Destructor:
VulkanPipelineLayout::~VulkanPipelineLayout()
{
    vkDestroyDescriptorSetLayout(logicalDevice->device, descriptorSetLayout, nullptr);
    vkDestroyPipelineLayout(logicalDevice->device, pipelineLayout, nullptr);
}