#include "vulkanGizmoPipeline.h"
#include "vulkanContext.h"
#include "vulkanDefaultPushConstant.h"
#include "vulkanGizmoRenderPass.h"
#include "vulkanMacros.h"
#include "vulkanRenderPassManager.h"
#include <array>
#include <stdexcept>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	GizmoPipeline::GizmoPipeline(
		const std::string& name,
		VkPipelineLayout vkPipelineLayout,
		emberCommon::RenderMode renderMode,
		const std::vector<char>& vertexCode,
		const std::vector<char>& fragmentCode,
		const std::vector<VkVertexInputBindingDescription>& vertexBindings,
		const std::vector<VkVertexInputAttributeDescription>& vertexAttributes)
	{
		// Create vertex and fragment shader modules from .spv files:
		VkShaderModule vertexShaderModule = CreateShaderModule(vertexCode, "ShaderModule_GizmoVertex_" + name);
		VkShaderModule fragmentShaderModule = CreateShaderModule(fragmentCode, "ShaderModule_GizmoFragment_" + name);

		// Create pipeline:
		CreatePipeline(vkPipelineLayout, vertexShaderModule, fragmentShaderModule, vertexBindings, vertexAttributes, renderMode);

		// Destroy shader modules (only needed for pipeline creation):
		vkDestroyShaderModule(Context::GetVkDevice(), vertexShaderModule, nullptr);
		vkDestroyShaderModule(Context::GetVkDevice(), fragmentShaderModule, nullptr);
		NAME_VK_OBJECT(m_pipeline, "Pipeline_Gizmo_" + name);
	}
	GizmoPipeline::~GizmoPipeline()
	{

	}



	// Private methods:
	void GizmoPipeline::CreatePipeline(
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
		VkBool32 blendEnable;
		switch (renderMode)
		{
		case emberCommon::RenderMode::opaque:
			polygonMode = VK_POLYGON_MODE_FILL;
			cullMode = VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
			frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
			depthWriteEnable = VK_TRUE;
			blendEnable = VK_FALSE;
			break;
		case emberCommon::RenderMode::transparent:
			polygonMode = VK_POLYGON_MODE_FILL;
			cullMode = VkCullModeFlagBits::VK_CULL_MODE_NONE;
			frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
			depthWriteEnable = VK_FALSE;
			blendEnable = VK_TRUE;
			break;
		case emberCommon::RenderMode::skybox:
			polygonMode = VK_POLYGON_MODE_FILL;
			cullMode = VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
			frontFace = VK_FRONT_FACE_CLOCKWISE;
			depthWriteEnable = VK_FALSE;
			blendEnable = VK_FALSE;
			break;
		case emberCommon::RenderMode::wireframe:
			polygonMode = VK_POLYGON_MODE_LINE;
			cullMode = VkCullModeFlagBits::VK_CULL_MODE_NONE;
			frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
			depthWriteEnable = VK_TRUE;
			blendEnable = VK_FALSE;
			break;
		default:
			throw std::runtime_error("GizmoPipeline::CreatePipeline(...) failed. Unsupported render mode.");
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

		// Multiple viewports and scissors can be used for multiview rendering (VR). Requires multiview feature:
		VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		// Rasterization:
		VkPipelineRasterizationStateCreateInfo rasterizationState = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
		rasterizationState.polygonMode = polygonMode;       // fill=fill triangles, line=draw lines, point=draw points. Line is useful for wireframe rendering.
		rasterizationState.cullMode = cullMode;             // which face to cull.
		rasterizationState.frontFace = frontFace;           // which face of triangle is front: 123 or 132?
		rasterizationState.lineWidth = 1.0f;                // width of lines. Bigger 1.0f requires wideLines feature.
		rasterizationState.depthClampEnable = VK_FALSE;     // clamping fragments instead of discarding them is useful for shadow mapping. Requires depthClamp feature.
		rasterizationState.depthBiasEnable = VK_FALSE;      // optional.
		rasterizationState.depthBiasConstantFactor = 0.0f;  // optional.
		rasterizationState.depthBiasClamp = 0.0f;           // optional.
		rasterizationState.depthBiasSlopeFactor = 0.0f;     // optional.

		// Multisampling:
		VkPipelineMultisampleStateCreateInfo multisampleState = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
		multisampleState.sampleShadingEnable = VK_FALSE;
		multisampleState.rasterizationSamples = Context::GetMsaaSamples();
		multisampleState.minSampleShading = 1.0f;           // optional.
		multisampleState.pSampleMask = nullptr;             // optional.
		multisampleState.alphaToCoverageEnable = VK_FALSE;  // optional.
		multisampleState.alphaToOneEnable = VK_FALSE;       // optional.

		// Depth and stencil testing:
		VkPipelineDepthStencilStateCreateInfo depthState = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
		depthState.depthTestEnable = VK_TRUE;
		depthState.depthWriteEnable = depthWriteEnable;
		depthState.depthCompareOp = VK_COMPARE_OP_LESS;
		depthState.depthBoundsTestEnable = VK_FALSE;
		depthState.stencilTestEnable = VK_FALSE;

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
		pipelineInfo.pDepthStencilState = &depthState;          // depth and stencil testing.
		pipelineInfo.pColorBlendState = &colorBlendState;       // color blending.
		pipelineInfo.pDynamicState = &dynamicState;             // dynamic states: viewport, scissor and cull mode.
		pipelineInfo.layout = vkPipelineLayout;
		pipelineInfo.renderPass = RenderPassManager::GetGizmoRenderPass()->GetVkRenderPass();
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;       // can be used to create a new pipeline based on an existing one.
		pipelineInfo.basePipelineIndex = -1;                    // do not inherit from existing pipeline.

		VKA(vkCreateGraphicsPipelines(Context::GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline));
	}
}