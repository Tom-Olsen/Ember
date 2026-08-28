#include "vulkanDeferredLightingPipeline.h"
#include "commonDeferredLightingRenderState.h"
#include "vulkanContext.h"
#include "vulkanConvertMaterialRenderState.h"
#include "vulkanDeferredLightingRenderPass.h"
#include "vulkanMacros.h"
#include "vulkanRenderPassManager.h"
#include <array>



namespace vulkanRendererBackend
{
	// Public methods:
    // Constructor/Destructor:
	DeferredLightingPipeline::DeferredLightingPipeline(
		VkPipelineLayout vkPipelineLayout,
		const std::vector<char>& vertexCode,
		const std::vector<char>& fragmentCode,
		const std::string& debugName)
	{
        // Create vertex and fragment shader modules from .spv files:
		VkShaderModule vertexShaderModule = CreateShaderModule(vertexCode, "ShaderModule_DeferredLightingVertex_" + debugName);
		VkShaderModule fragmentShaderModule = CreateShaderModule(fragmentCode, "ShaderModule_DeferredLightingFragment_" + debugName);

        // Create pipeline:
		CreatePipeline(vkPipelineLayout, vertexShaderModule, fragmentShaderModule);

        // Destroy shader modules (only needed for pipeline creation):
		vkDestroyShaderModule(Context::GetVkDevice(), vertexShaderModule, nullptr);
		vkDestroyShaderModule(Context::GetVkDevice(), fragmentShaderModule, nullptr);
		NAME_VK_OBJECT(m_pipeline, "Pipeline_DeferredLighting_" + debugName);
	}
	DeferredLightingPipeline::~DeferredLightingPipeline()
	{

	}



	// Private methods:
	void DeferredLightingPipeline::CreatePipeline(
		VkPipelineLayout vkPipelineLayout,
		const VkShaderModule& vertexShaderModule,
		const VkShaderModule& fragmentShaderModule)
	{
		const emberCommon::DeferredLightingRenderState renderState;

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

        // Input assembly:
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
		inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        // Multiple viewports and scissors can be used for multiview rendering (VR). Requires multiview feature:
		VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

        // Rasterization:
		VkPipelineRasterizationStateCreateInfo rasterizationState = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
		rasterizationState.polygonMode = PolygonModeCommonToVulkan(renderState.polygonMode);
		rasterizationState.cullMode = CullModeCommonToVulkan(renderState.cullMode);
		rasterizationState.frontFace = FrontFaceCommonToVulkan(renderState.frontFace);
		rasterizationState.lineWidth = 1.0f;
		rasterizationState.depthClampEnable = VK_FALSE;
		rasterizationState.depthBiasEnable = VK_FALSE;

        // Multisampling:
		VkPipelineMultisampleStateCreateInfo multisampleState = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
		multisampleState.sampleShadingEnable = VK_FALSE;
		multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        // Color blending:
		VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
		colorBlendAttachmentState.blendEnable = renderState.blendEnable;
		colorBlendAttachmentState.colorWriteMask = ColorWriteMaskCommonToVulkan(renderState.colorWriteMask);
		colorBlendAttachmentState.srcColorBlendFactor = BlendFactorCommonToVulkan(renderState.srcColorBlendFactor);
		colorBlendAttachmentState.dstColorBlendFactor = BlendFactorCommonToVulkan(renderState.dstColorBlendFactor);
		colorBlendAttachmentState.colorBlendOp = BlendOpCommonToVulkan(renderState.colorBlendOp);
		colorBlendAttachmentState.srcAlphaBlendFactor = BlendFactorCommonToVulkan(renderState.srcAlphaBlendFactor);
		colorBlendAttachmentState.dstAlphaBlendFactor = BlendFactorCommonToVulkan(renderState.dstAlphaBlendFactor);
		colorBlendAttachmentState.alphaBlendOp = BlendOpCommonToVulkan(renderState.alphaBlendOp);
		
        // Color blending settings:
		VkPipelineColorBlendStateCreateInfo colorBlendState = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
		colorBlendState.attachmentCount = 1;
		colorBlendState.pAttachments = &colorBlendAttachmentState;
		colorBlendState.logicOpEnable = VK_FALSE;

        // Dynamic states, can be changed without recreating the pipeline:
		std::array<VkDynamicState, 2> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		VkPipelineDynamicStateCreateInfo dynamicState = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		VkGraphicsPipelineCreateInfo pipelineInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
		pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputState;
		pipelineInfo.pInputAssemblyState = &inputAssemblyState;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizationState;
		pipelineInfo.pMultisampleState = &multisampleState;
		pipelineInfo.pColorBlendState = &colorBlendState;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = vkPipelineLayout;
		pipelineInfo.renderPass = RenderPassManager::GetDeferredLightingRenderPass()->GetVkRenderPass();
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;		// can be used to create a new pipeline based on an existing one.
		pipelineInfo.basePipelineIndex = -1;					// do not inherit from existing pipeline.

		VKA(vkCreateGraphicsPipelines(Context::GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline));
	}
}