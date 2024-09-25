#include "vulkanPipelineLayout.h"
#include "macros.h"



// Constructor:
VulkanPipelineLayout::VulkanPipelineLayout(VulkanLogicalDevice* logicalDevice)
{
    this->logicalDevice = logicalDevice;

    std::array<VkDescriptorSetLayoutBinding,2> bindings;
    VkDescriptorSetLayoutBinding globalUniformObjectlayoutBinding = {};
    globalUniformObjectlayoutBinding.binding = 0;                             // binding point in shader
    globalUniformObjectlayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    globalUniformObjectlayoutBinding.descriptorCount = 1;                     // allows for arrays of descriptors
    globalUniformObjectlayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; // used in vertex shader
    globalUniformObjectlayoutBinding.pImmutableSamplers = nullptr;            // needed later for image sampling
    bindings[0] = globalUniformObjectlayoutBinding;

    VkDescriptorSetLayoutBinding imageSamplerLayoutBinding{};
    imageSamplerLayoutBinding.binding = 1;
    imageSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    imageSamplerLayoutBinding.descriptorCount = 1;
    imageSamplerLayoutBinding.pImmutableSamplers = nullptr;
    imageSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;        // used in fragment shader
    bindings[1] = imageSamplerLayoutBinding;

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
    descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    descriptorSetLayoutCreateInfo.pBindings = bindings.data();

    // VkDescriptorSetLayout describes the kind of descriptors that can be bound.
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