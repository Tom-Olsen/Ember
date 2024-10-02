//#include "vulkanPipelineLayout.h"
//#include "macros.h"
//
//
//
//// Constructor:
//VulkanPipelineLayout::VulkanPipelineLayout(VulkanLogicalDevice* logicalDevice, const std::vector<VkDescriptorSetLayoutBinding>& bindings)
//{
//    this->logicalDevice = logicalDevice;
//
//    //std::array<VkDescriptorSetLayoutBinding, 2> bindings{};
//    //{
//    //    // global uniform object layout binding:
//    //    bindings[0].binding = 0;                             // binding point in shader
//    //    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//    //    bindings[0].descriptorCount = 1;                     // allows for arrays of descriptors
//    //    bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT; // used in vertex shader
//    //    bindings[0].pImmutableSamplers = nullptr;            // needed later for image sampling
//    //
//    //    // image sampler layout binding:
//    //    bindings[1].binding = 1;
//    //    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//    //    bindings[1].descriptorCount = 1;
//    //    bindings[1].pImmutableSamplers = nullptr;
//    //    bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;  // used in fragment shader
//    //}
//
//    //std::vector<VkDescriptorSetLayoutBinding> bindings = material->GetDescriptorSetLayoutBindings();
//    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
//    descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
//    descriptorSetLayoutCreateInfo.pBindings = bindings.data();
//
//    // VkDescriptorSetLayout describes the kind of descriptors that can be bound.
//    VKA(vkCreateDescriptorSetLayout(logicalDevice->device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout));
//
//    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
//    pipelineLayoutCreateInfo.setLayoutCount = 1;
//    pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
//	pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
//	pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
//    vkCreatePipelineLayout(logicalDevice->device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
//}
//
//
//
//// Destructor:
//VulkanPipelineLayout::~VulkanPipelineLayout()
//{
//    vkDestroyDescriptorSetLayout(logicalDevice->device, descriptorSetLayout, nullptr);
//    vkDestroyPipelineLayout(logicalDevice->device, pipelineLayout, nullptr);
//}