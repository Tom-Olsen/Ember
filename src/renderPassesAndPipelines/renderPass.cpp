#include "renderPass.h"



RenderPass::~RenderPass()
{
	// Destroy framebuffers:
	for (uint32_t i = 0; i < framebuffers.size(); i++)
		vkDestroyFramebuffer(context->GetVkDevice(), framebuffers[i], nullptr);
	framebuffers.clear();

	// Destroy renderpass:
	vkDestroyRenderPass(context->GetVkDevice(), renderPass, nullptr);
}