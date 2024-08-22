#include <fstream>
#include <vector>
#include "logger.h"
#include "macros.h"
#include "vulkanPipeline.h"



// Constructor:
VulkanPipeline::VulkanPipeline(VulkanLogicalDevice* logicalDevice, VulkanRenderpass* renderpass, const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename)
{
    this->logicalDevice = logicalDevice;
    this->renderpass = renderpass;

    VkShaderModule vertexShaderModule = CreateShaderModule(vertexShaderFilename);
    VkShaderModule fragmentShaderModule = CreateShaderModule(fragmentShaderFilename);
    CreatePipeline(vertexShaderModule, fragmentShaderModule);
    vkDestroyShaderModule(logicalDevice->device, vertexShaderModule, nullptr);
    vkDestroyShaderModule(logicalDevice->device, fragmentShaderModule, nullptr);
}



// Destructor:
VulkanPipeline::~VulkanPipeline()
{
    vkDestroyPipeline(logicalDevice->device, pipeline, nullptr);
    vkDestroyPipelineLayout(logicalDevice->device, pipelineLayout, nullptr);
}



// Private:
VkShaderModule VulkanPipeline::CreateShaderModule(std::string shaderFilename)
{
    // Open shader file:
    std::ifstream file(shaderFilename, std::ios::binary);
    if (!file.is_open())
        LOG_CRITICAL("Error opening shader file!");

    // Get file size
    file.seekg(0, std::ios::end);
    size_t fileSize = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);

    // Copy data to buffer:
    std::vector<char> buffer(fileSize);
    file.read(buffer.data(), fileSize);

    // Create shader module:
    VkShaderModuleCreateInfo createInfo =
    {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,   // type of this struct
        .pNext = nullptr,                                       // pointer to extension-specific data
        .flags = 0,                                             // flags (none)
        .codeSize = fileSize,                                   // size of code in bytes
        .pCode = (uint32_t*)(buffer.data())                     // pointer to code
    };
    VkShaderModule shaderModule;
    VKA(vkCreateShaderModule(logicalDevice->device, &createInfo, nullptr, &shaderModule));

    // Close file and return:
    file.close();
    return shaderModule;
}

void VulkanPipeline::CreatePipeline(const VkShaderModule& vertexShaderModule, const VkShaderModule& fragmentShaderModule)
{
    VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    vertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderStageCreateInfo.module = vertexShaderModule;
    vertexShaderStageCreateInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    fragmentShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentShaderStageCreateInfo.module = fragmentShaderModule;
    fragmentShaderStageCreateInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[2] = { vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo };

    // Pipeline layout:
    {
        VkPipelineLayoutCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO, // type of this struct
            .pNext = nullptr,                                       // pointer to extension-specific data
            .flags = 0,                                             // flags (none)
            .setLayoutCount = 0,                                    // number of descriptor set layouts
            .pSetLayouts = nullptr,                                 // pointer to descriptor set layouts
            .pushConstantRangeCount = 0,                            // number of push constant ranges
            .pPushConstantRanges = nullptr                          // pointer to push constant ranges
        };
        vkCreatePipelineLayout(logicalDevice->device, &createInfo, nullptr, &pipelineLayout);
    }

    // Pipeline:
    {

        VkVertexInputBindingDescription bindingDescriptions[2];
        {
            // position binding:
            bindingDescriptions[0].binding = 0;
            bindingDescriptions[0].stride = sizeof(Float3);  // Assuming each vertex is a vec3
            bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            // color binding:
            bindingDescriptions[1].binding = 1;
            bindingDescriptions[1].stride = sizeof(Float4);  // Assuming each color is a vec4
            bindingDescriptions[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        }

        VkVertexInputAttributeDescription attributeDescriptions[2];
        {
            // position attribute:
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = 0;

            // color attribute:
            attributeDescriptions[1].binding = 1;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[1].offset = 0;
        }

        VkPipelineVertexInputStateCreateInfo vertexInputState = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        vertexInputState.vertexBindingDescriptionCount = 2;
        vertexInputState.pVertexBindingDescriptions = bindingDescriptions;
        vertexInputState.vertexAttributeDescriptionCount = 2;
        vertexInputState.pVertexAttributeDescriptions = attributeDescriptions;

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizationState = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizationState.lineWidth = 1.0f;

        VkPipelineMultisampleStateCreateInfo multisampleState = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampleState.minSampleShading = 1.0f;

        VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
        colorBlendAttachmentState.blendEnable = VK_FALSE;
        colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo colorBlendState = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        colorBlendState.attachmentCount = 1;
        colorBlendState.pAttachments = &colorBlendAttachmentState;

        VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo dynamicState = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        dynamicState.dynamicStateCount = ARRAY_SIZE(dynamicStates);
        dynamicState.pDynamicStates = dynamicStates;

        VkGraphicsPipelineCreateInfo createInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
        createInfo.stageCount = 2;
        createInfo.pStages = shaderStages;
        createInfo.pVertexInputState = &vertexInputState;
        createInfo.pInputAssemblyState = &inputAssemblyState;
        createInfo.pViewportState = &viewportState;
        createInfo.pRasterizationState = &rasterizationState;
        createInfo.pMultisampleState = &multisampleState;
        createInfo.pColorBlendState = &colorBlendState;
        createInfo.pDynamicState = &dynamicState;
        createInfo.layout = pipelineLayout;
        createInfo.renderPass = renderpass->renderpass;
        createInfo.subpass = 0;
        createInfo.basePipelineIndex = -1;

        vkCreateGraphicsPipelines(logicalDevice->device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline);
    }
}