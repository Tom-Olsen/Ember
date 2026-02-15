#include "vulkanShadowPipeline.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include "vulkanRenderPassManager.h"
#include "vulkanShadowPushConstant.h"
#include "vulkanShadowRenderPass.h"
#include "vulkanVertexLayout.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
    // Constructor/Destructor:
    template<typename vertexLayout>
    ShadowPipeline<vertexLayout>::ShadowPipeline(const std::string& name, uint32_t shadowMapResolution, const std::vector<char>& vertexCode, const std::vector<VkDescriptorSetLayout>& vkDescriptorSetLayouts, const std::vector<VkVertexInputBindingDescription>& vertexBindings, const std::vector<VkVertexInputAttributeDescription>& vertexAttributes)
    {
        m_name = name;

        // Create pipeline Layout:
        CreatePipelineLayout(vkDescriptorSetLayouts);

        // Create vertex and fragment shader modules from .spv files:
        VkShaderModule vertexShaderModule = CreateShaderModule(vertexCode);

        // Create pipeline:
        CreatePipeline(shadowMapResolution, vertexShaderModule, shaderReflection);

        // Destroy shader modules (only needed for pipeline creation):
        vkDestroyShaderModule(Context::GetVkDevice(), vertexShaderModule, nullptr);
        NAME_VK_OBJECT(m_pipeline, m_name + "ShadowPipeline");
    }
    template<typename vertexLayout>
    ShadowPipeline<vertexLayout>::~ShadowPipeline()
    {

    }



    // Private:
    template<typename vertexLayout>
    void ShadowPipeline<vertexLayout>::CreatePipelineLayout(const std::vector<VkDescriptorSetLayout>& vkDescriptorSetLayouts)
    {
        // Push constants layout:
        VkPushConstantRange pushConstantRange = {};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(ShadowPushConstant);

        // Pipeline layout:
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(vkDescriptorSetLayouts.size());
        pipelineLayoutCreateInfo.pSetLayouts = vkDescriptorSetLayouts.data();
        pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
        pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

        VKA(vkCreatePipelineLayout(Context::GetVkDevice(), &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout));
        NAME_VK_OBJECT(m_pipelineLayout, m_name + "ShadowPipelineLayout");
    }
    template<typename vertexLayout>
    void ShadowPipeline<vertexLayout>::CreatePipeline(uint32_t shadowMapResolution, const VkShaderModule& vertexShaderModule, const std::vector<VkVertexInputBindingDescription>& vertexBindings, const std::vector<VkVertexInputAttributeDescription>& vertexAttributes)
    {
        // Vertex shader:
        VkPipelineShaderStageCreateInfo vertexShaderStageInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
        vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertexShaderStageInfo.module = vertexShaderModule;
        vertexShaderStageInfo.pName = "main";

        // Vertex input:
        VkPipelineVertexInputStateCreateInfo vertexInputState = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        vertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindings.size());
        vertexInputState.pVertexBindingDescriptions = vertexBindings.data();
        vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributes.size());
        vertexInputState.pVertexAttributeDescriptions = vertexAttributes.data();

        // Input assembly:
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        // Viewports and scissors:
        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)shadowMapResolution;
        viewport.height = (float)shadowMapResolution;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor = {};
        scissor.offset = { 0, 0 };
        scissor.extent = VkExtent2D{ shadowMapResolution,  shadowMapResolution };
        VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        // Rasterization:
        VkPipelineRasterizationStateCreateInfo rasterizationState = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;  // fill=fill triangles, line=draw lines, point=draw points. Line is useful for wireframe rendering
        rasterizationState.cullMode = VK_CULL_MODE_NONE;        // render both sides to fix shadow issue with one sides meshes.
        rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE; // which face of triangle is front: 123 or 132?
        rasterizationState.lineWidth = 1.0f;
        rasterizationState.depthClampEnable = Context::DepthClampEnabled();
        rasterizationState.depthBiasEnable = Context::DepthBiasClampEnabled();
        rasterizationState.depthBiasConstantFactor = Context::GetDepthBiasConstantFactor();  // Tweak this value based on the scene.
        rasterizationState.depthBiasClamp = Context::GetDepthBiasClamp();                    // Clamp value for equation below.
        rasterizationState.depthBiasSlopeFactor = Context::GetDepthBiasSlopeFactor();        // Slope scale bias to handle varying slopes in depth.
        rasterizationState.rasterizerDiscardEnable = VK_FALSE;	// If true, geometry never passes through rasterization stage.
        // Bias = (float) depthBiasConstantFactor * pow(exp(max_z_in_primitive) - r, 2) + depthBiasSlopeFactor * MaxDepthSlope;
        // r is the number of mantissa bits in the floating point representation (excluding the hidden bit); for example, 23 for float32.
        // MaxDepthSlope is approximately: max(abs(dz / dx), abs(dz / dy))

        // Multisampling:
        VkPipelineMultisampleStateCreateInfo multisampleState = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        multisampleState.sampleShadingEnable = VK_FALSE;				// enables or disable sample shading
        multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;	// specifies the number of samples used in rasterization

        // Depth and stencil testing:
        VkPipelineDepthStencilStateCreateInfo depthState = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
        depthState.depthTestEnable = VK_TRUE;             // depth of new fragments should be compared to the depth buffer to see if they should be discarded
        depthState.depthWriteEnable = VK_TRUE;            // new depth of fragments that pass the depth test should be written to the depth buffer
        depthState.depthCompareOp = VK_COMPARE_OP_LESS;   // comparison that is performed to keep or discard fragments. lower = closer to camera
        depthState.depthBoundsTestEnable = VK_FALSE;      // allows to keep only fragments in the below defined range
        depthState.stencilTestEnable = VK_FALSE;          // stencil buffer operations (not used yet)

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

        // Color blending settings:
        VkPipelineColorBlendStateCreateInfo colorBlendState = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        colorBlendState.attachmentCount = 0;	// no color blending for shadow mapping

        // Dynamic states, can be changed without recreating the pipeline:
        std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_DEPTH_BIAS };
        VkPipelineDynamicStateCreateInfo dynamicState = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkGraphicsPipelineCreateInfo pipelineInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
        pipelineInfo.stageCount = 1;								// only vertex shaders
        pipelineInfo.pStages = &vertexShaderStageInfo;				// shader stages pointer (only vertex shader)
        pipelineInfo.pVertexInputState = &vertexInputState;			// Buffer format
        pipelineInfo.pInputAssemblyState = &inputAssemblyState;		// Input assembler
        pipelineInfo.pViewportState = &viewportState;				// Viewport and scissor
        pipelineInfo.pRasterizationState = &rasterizationState;		// Rasterizer
        pipelineInfo.pMultisampleState = &multisampleState;			// Multisampling
        pipelineInfo.pDepthStencilState = &depthState;			    // Depth and stencil testing
        pipelineInfo.pColorBlendState = &colorBlendState;			// Color blending
		pipelineInfo.pDynamicState = &dynamicState;					// Dynamic states: depth bias 
        pipelineInfo.layout = m_pipelineLayout;
        pipelineInfo.renderPass = RenderPassManager::GetShadowRenderPass()->GetVkRenderPass();
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;

        VKA(vkCreateGraphicsPipelines(Context::GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline));
    }
}