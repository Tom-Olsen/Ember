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

    // Create vertex and fragment shader modules from .spv files:
    VkShaderModule vertexShaderModule = CreateShaderModule(vertexShaderFilename);
    VkShaderModule fragmentShaderModule = CreateShaderModule(fragmentShaderFilename);

    CreatePipeline(vertexShaderModule, fragmentShaderModule);

    // Destroy shader modules (only needed for pipeline creation):
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

// Create shader module (eg. vertex or fragment shader) from .spv file:
VkShaderModule VulkanPipeline::CreateShaderModule(std::string shaderFilename)
{
    // Open shader file:
    std::ifstream file(shaderFilename, std::ios::binary);
    if (!file.is_open())
        LOG_CRITICAL("Error opening shader file!");

    // Get file size:
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
    // Vertex shader:
    VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    vertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderStageCreateInfo.module = vertexShaderModule;
    vertexShaderStageCreateInfo.pName = "main";
    vertexShaderStageCreateInfo.pSpecializationInfo = nullptr;  // shader constants

    // Fragment shader:
    VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    fragmentShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentShaderStageCreateInfo.module = fragmentShaderModule;
    fragmentShaderStageCreateInfo.pName = "main";
    fragmentShaderStageCreateInfo.pSpecializationInfo = nullptr;  // shader constants

    VkPipelineShaderStageCreateInfo shaderStages[2] = { vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo };

    // Pipeline layout:
    {
        // Not really needed yet, but required for vulkan to work properly. So we leaf it empty
        // Used for uniform buffers, push constants, etc.
        VkPipelineLayoutCreateInfo createInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        vkCreatePipelineLayout(logicalDevice->device, &createInfo, nullptr, &pipelineLayout);
    }

    // Pipeline:
    {

        // binding = position of data in buffer
        // stride = number of bytes between two consecutive elements
        VkVertexInputBindingDescription bindingDescriptions[2];
        {
            // position binding:
            bindingDescriptions[0].binding = 0;
            bindingDescriptions[0].stride = sizeof(Float3);
            bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            // color binding:
            bindingDescriptions[1].binding = 1;
            bindingDescriptions[1].stride = sizeof(Float4);
            bindingDescriptions[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        }
        // => x0y0z0, x1y1z1, ..., r0g0b0a0, r1g1b1a1, ...

        // binding = same as above
        // location = which input in the vertex shader to load (same as location in glsl shader)
        // format = how to interpret the data
        // offset = data offset
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

        // topology = how to interpret the vertices, triangle list is the most flexible
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        // Multiple viewports and scissors can be used for multiview rendering (VR). Requires multiview feature
        VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizationState = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;  // fill=fill triangles, line=draw lines, point=draw points. Line is useful for wireframe rendering
        rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;    // which face to cull
        rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE; // which face of triangle is front: 123 or 132?
        rasterizationState.lineWidth = 1.0f;    // width of lines. Bigger 1.0f requires wideLines feature
        rasterizationState.depthClampEnable = VK_FALSE; // clamping fragments instead of discarding them is useful for shadow mapping. Requires depthClamp feature
        // Biasing depth values based on fragment's slope is useful for shadow mapping. Requires depthBias feature. We are not using it here
        rasterizationState.depthBiasEnable = VK_FALSE;
        rasterizationState.depthBiasConstantFactor = 0.0f;  // Optional
        rasterizationState.depthBiasClamp = 0.0f;           // Optional
        rasterizationState.depthBiasSlopeFactor = 0.0f;     // Optional

        // Allows for antialiasing along polygon edges by combining the fragment shader results of multiple polygons that rasterize to the same pixel.
        // Requires multisampling feature. We are not using it here
        VkPipelineMultisampleStateCreateInfo multisampleState = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        multisampleState.sampleShadingEnable = VK_FALSE;
        multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampleState.minSampleShading = 1.0f;           // Optional
        multisampleState.pSampleMask = nullptr;             // Optional
        multisampleState.alphaToCoverageEnable = VK_FALSE;  // Optional
        multisampleState.alphaToOneEnable = VK_FALSE;       // Optional

        // Configuration per attached framebuffer:
        VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
        colorBlendAttachmentState.blendEnable = VK_FALSE;
        colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

        // Color blending pseudo code:
        // if (blendEnable)
        // {
        //     finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb);
        //     finalColor.a = (srcAlphaBlendFactor * newColor.a) <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a);
        // }
        // else
        //     finalColor = newColor;
        // finalColor = finalColor & colorWriteMask;

        // Current settings give standard alpha blending:
        // finalColor.rgb = (srcAlpha * newColor.rgb) + ((1 - srcAlpha) * oldColor.rgb);
        // finalColor.a = newAlpha.a;

        // Global color blending settings:
        VkPipelineColorBlendStateCreateInfo colorBlendState = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        colorBlendState.attachmentCount = 1;
        colorBlendState.pAttachments = &colorBlendAttachmentState;
        colorBlendState.logicOpEnable = VK_FALSE;   // setting this to true overwrites the colorBlendAttachmentState settings and uses the logicOp
        colorBlendState.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlendState.blendConstants[0] = 0.0f;   // Optional
        colorBlendState.blendConstants[1] = 0.0f;   // Optional
        colorBlendState.blendConstants[2] = 0.0f;   // Optional
        colorBlendState.blendConstants[3] = 0.0f;   // Optional

        // Dynamic states, can be changed without recreating the pipeline:
        std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo dynamicState = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkGraphicsPipelineCreateInfo createInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
        createInfo.stageCount = 2;
        createInfo.pStages = shaderStages;
        createInfo.pVertexInputState = &vertexInputState;       // Buffer format
        createInfo.pInputAssemblyState = &inputAssemblyState;   // Input assembler
        createInfo.pViewportState = &viewportState;			    // Viewport and scissor
        createInfo.pRasterizationState = &rasterizationState;   // Rasterizer
        createInfo.pMultisampleState = &multisampleState;	    // Multisampling
        createInfo.pColorBlendState = &colorBlendState;		    // Color blending
        createInfo.pDynamicState = &dynamicState;			    // Dynamic states
        createInfo.layout = pipelineLayout;
        createInfo.renderPass = renderpass->renderpass;
        createInfo.subpass = 0;
        createInfo.basePipelineIndex = -1;

        vkCreateGraphicsPipelines(logicalDevice->device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline);
    }
}