#include "vulkanForwardPipeline.h"
#include "vulkanContext.h"
#include "vulkanDefaultPushConstant.h"
#include "vulkanForwardRenderPass.h"
#include "vulkanMacros.h"
#include "vulkanRenderPassManager.h"
#include <type_traits>



namespace vulkanRendererBackend
{
    // Public methods:
    // Constructor/Destructor:
    ForwardPipeline::ForwardPipeline(
        const std::string& name,
        VkPipelineLayout vkPipelineLayout,
        emberCommon::RenderMode renderMode,
        const std::vector<char>& vertexCode,
        const std::vector<char>& fragmentCode,
        const std::vector<VkVertexInputBindingDescription>& vertexBindings,
        const std::vector<VkVertexInputAttributeDescription>& vertexAttributes)
    {
        m_name = name;

        // Create vertex and fragment shader modules from .spv files:
        VkShaderModule vertexShaderModule = CreateShaderModule(vertexCode);
        VkShaderModule fragmentShaderModule = CreateShaderModule(fragmentCode);

        // Create pipeline:
        CreatePipeline(vkPipelineLayout, vertexShaderModule, fragmentShaderModule, vertexBindings, vertexAttributes, renderMode);

        // Destroy shader modules (only needed for pipeline creation):
        vkDestroyShaderModule(Context::GetVkDevice(), vertexShaderModule, nullptr);
        vkDestroyShaderModule(Context::GetVkDevice(), fragmentShaderModule, nullptr);
        NAME_VK_OBJECT(m_pipeline, m_name + "ForwardPipeline");
    }
    ForwardPipeline::~ForwardPipeline()
    {

    }



    // Private methods:
    void ForwardPipeline::CreatePipeline(
        VkPipelineLayout vkPipelineLayout,
        const VkShaderModule& vertexShaderModule,
        const VkShaderModule& fragmentShaderModule,
        const std::vector<VkVertexInputBindingDescription>& vertexBindings,
        const std::vector<VkVertexInputAttributeDescription>& vertexAttributes,
        emberCommon::RenderMode renderMode)
    {
        // Render mode specifics:
        VkPolygonMode polygonMode;
        VkCullModeFlagBits cullMode;
        VkFrontFace frontFace;
        VkBool32 depthWriteEnable;
        VkCompareOp depthCompareOp;
        VkBool32 blendEnable;
        switch (renderMode)
        {
            case emberCommon::RenderMode::opaque:
                polygonMode = VK_POLYGON_MODE_FILL;
                cullMode = VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
                frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
                depthWriteEnable = VK_TRUE;
                depthCompareOp = VK_COMPARE_OP_LESS;
                blendEnable = VK_FALSE;
                break;
            case emberCommon::RenderMode::transparent:
                polygonMode = VK_POLYGON_MODE_FILL;
                cullMode = VkCullModeFlagBits::VK_CULL_MODE_NONE;
                frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
                depthWriteEnable = VK_FALSE;
                depthCompareOp = VK_COMPARE_OP_LESS;
                blendEnable = VK_TRUE;
                break;
            case emberCommon::RenderMode::skybox:
                polygonMode = VK_POLYGON_MODE_FILL;
                cullMode = VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
                frontFace = VK_FRONT_FACE_CLOCKWISE;
                depthWriteEnable = VK_FALSE;
                depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
                blendEnable = VK_FALSE;
                break;
            case emberCommon::RenderMode::wireframe:
                polygonMode = VK_POLYGON_MODE_LINE;
                cullMode = VkCullModeFlagBits::VK_CULL_MODE_NONE;
                frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
                depthWriteEnable = VK_TRUE;
                depthCompareOp = VK_COMPARE_OP_LESS;
                blendEnable = VK_FALSE;
                break;
        }

        // Vertex shader:
        VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
        vertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertexShaderStageCreateInfo.module = vertexShaderModule;
        vertexShaderStageCreateInfo.pName = "main";

        // Fragment shader:
        VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
        fragmentShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragmentShaderStageCreateInfo.module = fragmentShaderModule;
        fragmentShaderStageCreateInfo.pName = "main";

        std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = { vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo };

        // Vertex input:
        VkPipelineVertexInputStateCreateInfo vertexInputState = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        vertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindings.size());
        vertexInputState.pVertexBindingDescriptions = vertexBindings.data();
        vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributes.size());
        vertexInputState.pVertexAttributeDescriptions = vertexAttributes.data();

        // Input assembly:
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;  // how to interpret the vertices, triangle list is the most flexible

        // Multiple viewports and scissors can be used for multiview rendering (VR). Requires multiview feature
        VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        // Rasterization:
        VkPipelineRasterizationStateCreateInfo rasterizationState = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        rasterizationState.polygonMode = polygonMode;       // fill=fill triangles, line=draw lines, point=draw points. Line is useful for wireframe rendering
        rasterizationState.cullMode = cullMode;             // which face to cull
        rasterizationState.frontFace = frontFace;           // which face of triangle is front: 123 or 132?
        rasterizationState.lineWidth = 1.0f;                // width of lines. Bigger 1.0f requires wideLines feature
        rasterizationState.depthClampEnable = VK_FALSE;     // clamping fragments instead of discarding them is useful for shadow mapping. Requires depthClamp feature.
        rasterizationState.depthBiasEnable = VK_FALSE;      // Optional
        rasterizationState.depthBiasConstantFactor = 0.0f;  // Optional
        rasterizationState.depthBiasClamp = 0.0f;           // Optional
        rasterizationState.depthBiasSlopeFactor = 0.0f;     // Optional

        // Multisampling:
        VkPipelineMultisampleStateCreateInfo multisampleState = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        multisampleState.sampleShadingEnable = VK_FALSE;
        multisampleState.rasterizationSamples = static_cast<VkSampleCountFlagBits>(Context::GetMsaaSamples());
        multisampleState.minSampleShading = 1.0f;           // Optional
        multisampleState.pSampleMask = nullptr;             // Optional
        multisampleState.alphaToCoverageEnable = VK_FALSE;  // Optional
        multisampleState.alphaToOneEnable = VK_FALSE;       // Optional

        // Depth and stencil testing:
        VkPipelineDepthStencilStateCreateInfo depthState = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
        depthState.depthTestEnable = VK_TRUE;               // depth of new fragments should be compared to the depth buffer to see if they should be discarded
        depthState.depthWriteEnable = depthWriteEnable;     // new depth of fragments that pass the depth test should be written to the depth buffer
        depthState.depthCompareOp = depthCompareOp;         // comparison that is performed to keep or discard fragments. lower = closer to camera
        depthState.depthBoundsTestEnable = VK_FALSE;        // allows to keep only fragments in the below defined range
        depthState.stencilTestEnable = VK_FALSE;            // stencil buffer operations (not used yet)

        // Color blending pseudo code:
        // if (blendEnable)
        // {
        //     finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb);
        //     finalColor.a = (srcAlphaBlendFactor * newColor.a) <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a);
        // }
        // else
        //     finalColor = newColor;
        // finalColor = finalColor & colorWriteMask;

        // Standard alpha blending:
        // finalColor.rgb = (srcAlpha * newColor.rgb) + ((1 - srcAlpha) * oldColor.rgb);
        // finalColor.a = newAlpha.a;

        // Color blending:
        VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
        colorBlendAttachmentState.blendEnable = blendEnable;
        colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

        // Color blending settings:
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

        VkGraphicsPipelineCreateInfo pipelineInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
        pipelineInfo.stageCount = shaderStages.size();          // vertex and fragment shaders
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputState;     // Buffer format
        pipelineInfo.pInputAssemblyState = &inputAssemblyState; // Input assembler
        pipelineInfo.pViewportState = &viewportState;           // Viewport and scissor
        pipelineInfo.pRasterizationState = &rasterizationState; // Rasterizer
        pipelineInfo.pMultisampleState = &multisampleState;     // Multisampling
        pipelineInfo.pDepthStencilState = &depthState;          // Depth and stencil testing
        pipelineInfo.pColorBlendState = &colorBlendState;       // Color blending
		pipelineInfo.pDynamicState = &dynamicState;             // Dynamic states: viewport and scissor
        pipelineInfo.layout = vkPipelineLayout;
        pipelineInfo.renderPass = RenderPassManager::GetForwardRenderPass()->GetVkRenderPass();
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;       // can be used to create a new pipeline based on an existing one
        pipelineInfo.basePipelineIndex = -1;					// do not inherit from existing pipeline

        VKA(vkCreateGraphicsPipelines(Context::GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline));
    }
}