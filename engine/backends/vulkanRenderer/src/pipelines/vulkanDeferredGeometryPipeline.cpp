#include "vulkanDeferredGeometryPipeline.h"
#include "commonDeferredGeometryRenderState.h"
#include "vulkanContext.h"
#include "vulkanConvertMaterialRenderState.h"
#include "vulkanDeferredGeometryRenderPass.h"
#include "vulkanDeferredRenderingContract.h"
#include "vulkanMacros.h"
#include "vulkanRenderPassManager.h"
#include <array>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	DeferredGeometryPipeline::DeferredGeometryPipeline(
		VkPipelineLayout vkPipelineLayout,
		const std::vector<char>& vertexCode,
		const std::vector<char>& fragmentCode,
		const std::vector<VkVertexInputBindingDescription>& vertexBindings,
		const std::vector<VkVertexInputAttributeDescription>& vertexAttributes,
		const std::string& debugName)
	{
        // Create vertex and fragment shader modules from .spv files:
		VkShaderModule vertexShaderModule = CreateShaderModule(vertexCode, "ShaderModule_DeferredVertex_" + debugName);
		VkShaderModule fragmentShaderModule = CreateShaderModule(fragmentCode, "ShaderModule_DeferredFragment_" + debugName);

        // Create pipeline:
		CreatePipeline(vkPipelineLayout, vertexShaderModule, fragmentShaderModule, vertexBindings, vertexAttributes);

        // Destroy shader modules (only needed for pipeline creation):
		vkDestroyShaderModule(Context::GetVkDevice(), vertexShaderModule, nullptr);
		vkDestroyShaderModule(Context::GetVkDevice(), fragmentShaderModule, nullptr);
		NAME_VK_OBJECT(m_pipeline, "Pipeline_DeferredGeometry" + debugName);
	}
	DeferredGeometryPipeline::~DeferredGeometryPipeline()
	{

	}



	// Private methods:
	void DeferredGeometryPipeline::CreatePipeline(
		VkPipelineLayout vkPipelineLayout,
		const VkShaderModule& vertexShaderModule,
		const VkShaderModule& fragmentShaderModule,
		const std::vector<VkVertexInputBindingDescription>& vertexBindings,
		const std::vector<VkVertexInputAttributeDescription>& vertexAttributes)
	{
		const emberCommon::DeferredGeometryRenderState renderState;

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

        // Depth and stencil testing:
		VkPipelineDepthStencilStateCreateInfo depthState = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
		depthState.depthTestEnable = renderState.depthTestEnable;									// depth of new fragments should be compared to the depth buffer to see if they should be discarded.
		depthState.depthWriteEnable = renderState.depthWriteEnable;									// new depth of fragments that pass the depth test should be written to the depth buffer.
		depthState.depthCompareOp = CompareOpCommonToVulkan(renderState.depthCompareOp);	// comparison that is performed to keep or discard fragments. lower = closer to camera.
		depthState.depthBoundsTestEnable = VK_FALSE;												// allows to keep only fragments in the below defined range.
		depthState.stencilTestEnable = VK_FALSE;													// stencil buffer operations (not used yet).

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
		std::array<VkPipelineColorBlendAttachmentState, deferredRenderingContract::colorAttachmentCount> colorBlendAttachmentStates;
		colorBlendAttachmentStates.fill(colorBlendAttachmentState);

        // Color blending settings:
		VkPipelineColorBlendStateCreateInfo colorBlendState = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
		colorBlendState.attachmentCount = static_cast<uint32_t>(colorBlendAttachmentStates.size());
		colorBlendState.pAttachments = colorBlendAttachmentStates.data();
		colorBlendState.logicOpEnable = VK_FALSE;

        // Dynamic states, can be changed without recreating the pipeline:
		std::array<VkDynamicState, 3> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_CULL_MODE };
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
		pipelineInfo.pDepthStencilState = &depthState;
		pipelineInfo.pColorBlendState = &colorBlendState;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = vkPipelineLayout;
		pipelineInfo.renderPass = RenderPassManager::GetDeferredGeometryRenderPass()->GetVkRenderPass();
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;		// can be used to create a new pipeline based on an existing one.
		pipelineInfo.basePipelineIndex = -1;					// do not inherit from existing pipeline.

		VKA(vkCreateGraphicsPipelines(Context::GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline));
	}
}