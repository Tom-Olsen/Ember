#include "renderPass.h"



RenderPass::~RenderPass()
{
	// Destroy framebuffers:
	for (uint32_t i = 0; i < framebuffers.size(); i++)
		vkDestroyFramebuffer(context->LogicalDevice(), framebuffers[i], nullptr);
	framebuffers.clear();

	// Destroy renderpass:
	vkDestroyRenderPass(context->LogicalDevice(), renderPass, nullptr);
}