#include "vulkanOutlinePipeline.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include "vulkanOutlineRenderPass.h"
#include "vulkanRenderPassManager.h"
#include <array>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	OutlinePipeline::OutlinePipeline(
		const std::string& name,
		VkPipelineLayout vkPipelineLayout,
		const std::vector<char>& vertexCode,
		const std::vector<char>& fragmentCode,
		const std::vector<VkVertexInputBindingDescription>& vertexBindings,
		const std::vector<VkVertexInputAttributeDescription>& vertexAttributes)
	{
		// Create vertex and fragment shader modules from .spv files:
		VkShaderModule vertexShaderModule = CreateShaderModule(vertexCode, "ShaderModule_OutlineVertex_" + name);
		VkShaderModule fragmentShaderModule = CreateShaderModule(fragmentCode, "ShaderModule_OutlineFragment_" + name);

		// Create pipeline:
		CreatePipeline(vkPipelineLayout, vertexShaderModule, fragmentShaderModule, vertexBindings, vertexAttributes);

		// Destroy shader modules (only needed for pipeline creation):
		vkDestroyShaderModule(Context::GetVkDevice(), vertexShaderModule, nullptr);
		vkDestroyShaderModule(Context::GetVkDevice(), fragmentShaderModule, nullptr);
		NAME_VK_OBJECT(m_pipeline, "Pipeline_Outline_" + name);
	}
	OutlinePipeline::~OutlinePipeline()
	{

	}



	// Private methods:
	void OutlinePipeline::CreatePipeline(
		VkPipelineLayout vkPipelineLayout,
		const VkShaderModule& vertexShaderModule,
		const VkShaderModule& fragmentShaderModule,
		const std::vector<VkVertexInputBindingDescription>& vertexBindings,
		const std::vector<VkVertexInputAttributeDescription>& vertexAttributes)
	{
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

		// Multiple viewports and scissors can be used for multiview rendering (VR). Requires multiview feature:
		VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		// Rasterization:
		VkPipelineRasterizationStateCreateInfo rasterizationState = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
		rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;          // fill=fill triangles, line=draw lines, point=draw points. Line is useful for wireframe rendering.
		rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;            // which face to cull.
		rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // which face of triangle is front: 123 or 132?
		rasterizationState.lineWidth = 1.0f;                            // width of lines. Bigger 1.0f requires wideLines feature.
		rasterizationState.depthClampEnable = VK_FALSE;                 // clamping fragments instead of discarding them is useful for shadow mapping. Requires depthClamp feature.
		rasterizationState.depthBiasEnable = VK_FALSE;                  // optional.
		rasterizationState.depthBiasConstantFactor = 0.0f;              // optional.
		rasterizationState.depthBiasClamp = 0.0f;                       // optional.
		rasterizationState.depthBiasSlopeFactor = 0.0f;                 // optional.
		rasterizationState.rasterizerDiscardEnable = VK_FALSE;

		// Multisampling:
		VkPipelineMultisampleStateCreateInfo multisampleState = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
		multisampleState.sampleShadingEnable = VK_FALSE;
		multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleState.minSampleShading = 1.0f;
		multisampleState.pSampleMask = nullptr;
		multisampleState.alphaToCoverageEnable = VK_FALSE;
		multisampleState.alphaToOneEnable = VK_FALSE;

		// Color blending:
		VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
		colorBlendAttachmentState.blendEnable = VK_FALSE;
		colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT;
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
		colorBlendState.logicOpEnable = VK_FALSE;   // setting this to true overwrites the colorBlendAttachmentState settings and uses the logicOp.
		colorBlendState.logicOp = VK_LOGIC_OP_COPY; // optional.
		colorBlendState.blendConstants[0] = 0.0f;   // optional.
		colorBlendState.blendConstants[1] = 0.0f;   // optional.
		colorBlendState.blendConstants[2] = 0.0f;   // optional.
		colorBlendState.blendConstants[3] = 0.0f;   // optional.

        // Dynamic states, can be changed without recreating the pipeline:
		std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_CULL_MODE };
		VkPipelineDynamicStateCreateInfo dynamicState = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		VkGraphicsPipelineCreateInfo pipelineInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
		pipelineInfo.stageCount = shaderStages.size();          // vertex and fragment shaders.
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputState;     // buffer format.
		pipelineInfo.pInputAssemblyState = &inputAssemblyState; // onput assembler.
		pipelineInfo.pViewportState = &viewportState;           // viewport and scissor.
		pipelineInfo.pRasterizationState = &rasterizationState; // rasterizer.
		pipelineInfo.pMultisampleState = &multisampleState;     // multisampling.
		pipelineInfo.pDepthStencilState = nullptr;              // depth and stencil testing.
		pipelineInfo.pColorBlendState = &colorBlendState;       // color blending.
		pipelineInfo.pDynamicState = &dynamicState;             // dynamic states: viewport, scissor and cull mode.
		pipelineInfo.layout = vkPipelineLayout;
		pipelineInfo.renderPass = RenderPassManager::GetOutlineRenderPass()->GetVkRenderPass();
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;       // can be used to create a new pipeline based on an existing one.
		pipelineInfo.basePipelineIndex = -1;                    // do not inherit from existing pipeline.

		VKA(vkCreateGraphicsPipelines(Context::GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline));
	}
}