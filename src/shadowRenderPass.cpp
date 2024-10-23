#include <fstream>
#include "shadowRenderPass.h"
#include "pushConstantObject.h"
#include "vulkanHelper.h"
#include "macros.h"



// static members:
uint32_t ShadowRenderPass::shadowMapWidth = 2048;
uint32_t ShadowRenderPass::shadowMapHeight = 2048;



// Constructor:
ShadowRenderPass::ShadowRenderPass(VulkanContext* context)
{
	this->context = context;

	CreateShadowMapImage();
	CreateRenderpass();
	CreateFramebuffers();
}



// Destructor:
ShadowRenderPass::~ShadowRenderPass()
{
	// Destroy framebuffers:
	for (uint32_t i = 0; i < context->swapchain->images.size(); i++)
		vkDestroyFramebuffer(context->LogicalDevice(), framebuffers[i], nullptr);
	framebuffers.clear();

	// Destroy renderpass:
	vkDestroyRenderPass(context->LogicalDevice(), renderpass, nullptr);
}



// Private:
void ShadowRenderPass::CreateShadowMapImage()
{
	// Subresource range:
	VkImageSubresourceRange subresourceRange;
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = 1;
	subresourceRange.baseArrayLayer = 0;
	subresourceRange.layerCount = 1;

	// Image info:
	VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = shadowMapWidth;
	imageInfo.extent.height = shadowMapHeight;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = shadowMapFormat;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = 0;

	// Allocation info:
	VmaAllocationCreateInfo allocationInfo = {};
	allocationInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
	allocationInfo.flags = 0;
	allocationInfo.requiredFlags = 0;
	allocationInfo.preferredFlags = 0;

	shadowMap = std::make_unique<VmaImage>(context, imageInfo, allocationInfo, subresourceRange);
}
void ShadowRenderPass::CreateRenderpass()
{
	// Attachment description:
	VkAttachmentDescription attachment = {};
	attachment.format = shadowMapFormat;
	attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;					// clear framebuffer to black before rendering
	attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;					// store for later render passes
	attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;			// do not use stencils
	attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;		// do not use stencils
	attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;				// we don't care about initial layout of the image
	attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;	// will be read in a shader

	// Attachment reference:
	VkAttachmentReference shadowMapAttachmentReference = {};
	shadowMapAttachmentReference.attachment = 0;
	shadowMapAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// Subpass description:
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.pDepthStencilAttachment = &shadowMapAttachmentReference;

	// Create render pass:
	VkRenderPassCreateInfo createInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
	createInfo.attachmentCount = 1;
	createInfo.pAttachments = &attachment;
	createInfo.subpassCount = 1;
	createInfo.pSubpasses = &subpass;
	VKA(vkCreateRenderPass(context->LogicalDevice(), &createInfo, nullptr, &renderpass));
}
void ShadowRenderPass::CreateFramebuffers()
{
	// One framebuffer per swapchain image:
	framebuffers.resize(context->swapchain->images.size());
	for (uint32_t i = 0; i < context->swapchain->images.size(); i++)
	{
		VkFramebufferCreateInfo framebufferInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
		framebufferInfo.renderPass = renderpass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = &shadowMap->imageView;
		framebufferInfo.width = shadowMapWidth;
		framebufferInfo.height = shadowMapHeight;
		framebufferInfo.layers = 1;
		vkCreateFramebuffer(context->LogicalDevice(), &framebufferInfo, nullptr, &framebuffers[i]);
	}
}



//void ShadowMap::CreatePipelineLayout()
//{
//	// Descriptor set layout binding:
//	VkDescriptorSetLayoutBinding binding = {};
//	binding.binding = 0;
//	binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//	binding.descriptorCount = 1;
//	binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//	binding.pImmutableSamplers = nullptr;
//
//	// Descriptor set layout:
//	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
//	descriptorSetLayoutCreateInfo.bindingCount = 1;
//	descriptorSetLayoutCreateInfo.pBindings = &binding;
//	VKA(vkCreateDescriptorSetLayout(context->LogicalDevice(), &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout));
//
//	// Push constants layout (not needed, but basicly free):
//	VkPushConstantRange pushConstantRange = {};
//	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//	pushConstantRange.offset = 0;
//	pushConstantRange.size = sizeof(PushConstantObject);
//
//	// Pipeline layout:
//	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
//	pipelineLayoutCreateInfo.setLayoutCount = 1;
//	pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
//	pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
//	pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
//	vkCreatePipelineLayout(context->LogicalDevice(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
//}



//void ShadowMap::CreatePipeline()
//{
//	// Create shader module from .spv file:
//	std::vector<char> code = ReadShaderCode("../shaders/shadow.spv");
//	VkShaderModuleCreateInfo createInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
//	createInfo.codeSize = code.size();
//	createInfo.pCode = (uint32_t*)(code.data());
//	VkShaderModule vertexShaderModule;
//	VKA(vkCreateShaderModule(context->LogicalDevice(), &createInfo, nullptr, &vertexShaderModule));
//
//	// Shader stage:
//	VkPipelineShaderStageCreateInfo vertexShaderStageInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
//	vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
//	vertexShaderStageInfo.module = vertexShaderModule;
//	vertexShaderStageInfo.pName = "main";
//
//	// TODO: get this from mesh class:
//	// Vertex input binding:
//	VkVertexInputBindingDescription bindingDescriptions = {};
//	bindingDescriptions.binding = 0;
//	bindingDescriptions.stride = sizeof(Float3);
//	bindingDescriptions.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
//
//	// TODO: get this from mesh class:
//	// Vertex input attribute:
//	VkVertexInputAttributeDescription attributeDescriptions = {};
//	attributeDescriptions.binding = 0;
//	attributeDescriptions.location = 0;
//	attributeDescriptions.format = VK_FORMAT_R32G32B32_SFLOAT;
//	attributeDescriptions.offset = 0;
//
//	// Vertex input state:
//	VkPipelineVertexInputStateCreateInfo vertexInputState = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
//	vertexInputState.vertexBindingDescriptionCount = 1;
//	vertexInputState.pVertexBindingDescriptions = &bindingDescriptions;
//	vertexInputState.vertexAttributeDescriptionCount = 1;
//	vertexInputState.pVertexAttributeDescriptions = &attributeDescriptions;
//
//
//
//	// topology = how to interpret the vertices, triangle list is the most flexible
//	VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
//	inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
//	inputAssemblyState.primitiveRestartEnable = VK_FALSE;	// disable the restart of primitives types
//
//	// Viewports and scissors:
//	VkViewport viewport = {};
//	viewport.x = 0.0f;
//	viewport.y = 0.0f;
//	viewport.width = static_cast<float>(shadowMapWidth);
//	viewport.height = static_cast<float>(shadowMapHeight);
//	viewport.minDepth = 0.0f;
//	viewport.maxDepth = 1.0f;
//	VkRect2D scissor = {};
//	scissor.offset = { 0, 0 };
//	scissor.extent = VkExtent2D{ shadowMapWidth,shadowMapHeight };
//	VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
//	viewportState.viewportCount = 1;
//	viewportState.pViewports = &viewport;
//	viewportState.scissorCount = 1;
//	viewportState.pScissors = &scissor;
//
//	// Rasterization:
//	VkPipelineRasterizationStateCreateInfo rasterizationState = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
//	rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;  // fill=fill triangles, line=draw lines, point=draw points. Line is useful for wireframe rendering
//	rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;    // which face to cull
//	rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE; // which face of triangle is front: 123 or 132?
//	rasterizationState.depthClampEnable = context->DepthClampEnabled() ? VK_TRUE : VK_FALSE; // clamp fragments outside near/far planes
//	rasterizationState.depthBiasEnable = VK_TRUE;
//	rasterizationState.depthBiasConstantFactor = 1.25f;		// Tweak this value based on the scene.
//	rasterizationState.depthBiasClamp = 0.0f;
//	rasterizationState.depthBiasSlopeFactor = 1.75f;		// Slope scale bias to handle varying slopes in depth.
//	rasterizationState.rasterizerDiscardEnable = VK_FALSE;	// If true, geometry never passes through rasterization stage.
//	rasterizationState.lineWidth = 1.0f;
//
//	// Multisampling:
//	VkPipelineMultisampleStateCreateInfo multisampleState = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
//	multisampleState.sampleShadingEnable = VK_FALSE;				// enables or disable sample shading
//	multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;	// specifies the number of samples used in rasterization
//
//	// Depth and stencil testing:
//	VkPipelineDepthStencilStateCreateInfo depthStencil = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
//	depthStencil.depthTestEnable = VK_TRUE;             // depth of new fragments should be compared to the depth buffer to see if they should be discarded
//	depthStencil.depthWriteEnable = VK_TRUE;            // new depth of fragments that pass the depth test should be written to the depth buffer
//	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;   // comparison that is performed to keep or discard fragments. lower = closer to camera
//	depthStencil.depthBoundsTestEnable = VK_FALSE;      // allows to keep only fragments in the below defined range
//	depthStencil.stencilTestEnable = VK_FALSE;          // stencil buffer operations (not used yet)
//
//	// Configuration per attached framebuffer:
//	VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
//	colorBlendAttachmentState.blendEnable = VK_FALSE;
//	colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
//	colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
//	colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
//	colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
//	colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
//	colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
//	colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
//
//	// Global color blending settings:
//	VkPipelineColorBlendStateCreateInfo colorBlendState = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
//	colorBlendState.attachmentCount = 0;	// no color blending for shadow mapping
//
//	VkGraphicsPipelineCreateInfo pipelineInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
//	pipelineInfo.stageCount = 1;								// only vertex shaders
//	pipelineInfo.pStages = &vertexShaderStageInfo;				// shader stages pointer (only vertex shader)
//	pipelineInfo.pVertexInputState = &vertexInputState;			// Buffer format
//	pipelineInfo.pInputAssemblyState = &inputAssemblyState;		// Input assembler
//	pipelineInfo.pViewportState = &viewportState;				// Viewport and scissor
//	pipelineInfo.pRasterizationState = &rasterizationState;		// Rasterizer
//	pipelineInfo.pMultisampleState = &multisampleState;			// Multisampling
//	pipelineInfo.pDepthStencilState = &depthStencil;			// Depth and stencil testing
//	pipelineInfo.pColorBlendState = &colorBlendState;			// Color blending
//	pipelineInfo.pDynamicState = nullptr;						// no dynamic states	
//	pipelineInfo.layout = pipelineLayout;
//	pipelineInfo.renderPass = renderpass;
//	pipelineInfo.subpass = 0;
//	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
//	pipelineInfo.basePipelineIndex = -1;
//
//	VKA(vkCreateGraphicsPipelines(context->LogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline));
//	vkDestroyShaderModule(context->LogicalDevice(), vertexShaderModule, nullptr);
//}