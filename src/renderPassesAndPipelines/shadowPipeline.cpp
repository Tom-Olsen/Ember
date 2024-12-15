#include "shadowPipeline.h"
#include "mesh.h"
#include "renderPass.h"
#include "renderPassManager.h"
#include "shadowPushConstant.h"
#include "shadowRenderPass.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"



// Constructor/Destructor:
ShadowPipeline::ShadowPipeline(VulkanContext* pContext,
    const std::vector<char>& vertexCode,
    const std::vector<VkDescriptorSetLayoutBinding>& descriptorSetBindings,
    const std::vector<VkVertexInputBindingDescription>& inputBindingDescriptions,
    const std::vector<VkVertexInputAttributeDescription>& inputAttributeDescriptions)
{
    m_pContext = pContext;

    // Create pipeline Layout:
    CreatePipelineLayout(descriptorSetBindings);

    // Create vertex and fragment shader modules from .spv files:
    VkShaderModule vertexShaderModule = CreateShaderModule(vertexCode);

    // Create pipeline:
    CreatePipeline(vertexShaderModule, inputBindingDescriptions, inputAttributeDescriptions);

    // Destroy shader modules (only needed for pipeline creation):
    vkDestroyShaderModule(m_pContext->GetVkDevice(), vertexShaderModule, nullptr);
}
ShadowPipeline::~ShadowPipeline()
{

}



// Private:
void ShadowPipeline::CreatePipelineLayout(const std::vector<VkDescriptorSetLayoutBinding>& descriptorSetBindings)
{
    // Descriptor set layout:
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
    descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(descriptorSetBindings.size());
    descriptorSetLayoutCreateInfo.pBindings = descriptorSetBindings.data();
    VKA(vkCreateDescriptorSetLayout(m_pContext->GetVkDevice(), &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayout));

    // Push constants layout:
    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(ShadowPushConstant);

    // Pipeline layout:
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &m_descriptorSetLayout;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
    vkCreatePipelineLayout(m_pContext->GetVkDevice(), &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout);
}
void ShadowPipeline::CreatePipeline(const VkShaderModule& vertexShaderModule,
    const std::vector<VkVertexInputBindingDescription>& inputBindingDescriptions,
    const std::vector<VkVertexInputAttributeDescription>& inputAttributeDescriptions)
{
    // Vertex shader:
    VkPipelineShaderStageCreateInfo vertexShaderStageInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderStageInfo.module = vertexShaderModule;
    vertexShaderStageInfo.pName = "main";

    // Vertex input:
    VkPipelineVertexInputStateCreateInfo vertexInputState = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
    vertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(inputBindingDescriptions.size());
    vertexInputState.pVertexBindingDescriptions = inputBindingDescriptions.data();
    vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(inputAttributeDescriptions.size());
    vertexInputState.pVertexAttributeDescriptions = inputAttributeDescriptions.data();

    // Input assembly:
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
    inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    // Viewports and scissors:
    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(ShadowRenderPass::s_shadowMapWidth);
    viewport.height = static_cast<float>(ShadowRenderPass::s_shadowMapHeight);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = VkExtent2D{ ShadowRenderPass::s_shadowMapWidth, ShadowRenderPass::s_shadowMapHeight };
    VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    // Rasterization:
    VkPipelineRasterizationStateCreateInfo rasterizationState = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
    rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;  // fill=fill triangles, line=draw lines, point=draw points. Line is useful for wireframe rendering
    rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;    // which face to cull
    rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE; // which face of triangle is front: 123 or 132?
    rasterizationState.lineWidth = 1.0f;
    rasterizationState.depthClampEnable = m_pContext->DepthClampEnabled();
    rasterizationState.depthBiasEnable = m_pContext->DepthBiasEnabled();
    rasterizationState.depthBiasConstantFactor = 0.0f;		// Tweak this value based on the scene.
    rasterizationState.depthBiasClamp = 0.0f;
    rasterizationState.depthBiasSlopeFactor = -1.0f;		// Slope scale bias to handle varying slopes in depth.
    rasterizationState.rasterizerDiscardEnable = VK_FALSE;	// If true, geometry never passes through rasterization stage.

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

    VkGraphicsPipelineCreateInfo pipelineInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
    pipelineInfo.stageCount = 1;								// only vertex shaders
    pipelineInfo.pStages = &vertexShaderStageInfo;				// shader stages pointer (only vertex shader)
    pipelineInfo.pVertexInputState = &vertexInputState;			// Buffer format
    pipelineInfo.pInputAssemblyState = &inputAssemblyState;		// Input assembler
    pipelineInfo.pViewportState = &viewportState;				// Viewport and scissor
    pipelineInfo.pRasterizationState = &rasterizationState;		// Rasterizer
    pipelineInfo.pMultisampleState = &multisampleState;			// Multisampling
    pipelineInfo.pDepthStencilState = &depthState;			// Depth and stencil testing
    pipelineInfo.pColorBlendState = &colorBlendState;			// Color blending
    pipelineInfo.pDynamicState = nullptr;						// no dynamic states	
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = RenderPassManager::GetRenderPass("shadowRenderPass")->GetVkRenderPass();
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    VKA(vkCreateGraphicsPipelines(m_pContext->GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline));
}