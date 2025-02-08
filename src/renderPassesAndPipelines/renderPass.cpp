#include "renderPass.h"
#include "vulkanContext.h"



namespace emberEngine
{
	// Virtual Destructor:
	RenderPass::~RenderPass()
	{
		// Destroy framebuffers:
		for (uint32_t i = 0; i < m_framebuffers.size(); i++)
			vkDestroyFramebuffer(m_pContext->GetVkDevice(), m_framebuffers[i], nullptr);
		m_framebuffers.clear();

		// Destroy renderpass:
		vkDestroyRenderPass(m_pContext->GetVkDevice(), m_renderPass, nullptr);
	}



	// Public methods:
	const VkRenderPass& RenderPass::GetVkRenderPass() const
	{
		return m_renderPass;
	}
	const std::vector<VkFramebuffer>& RenderPass::GetFramebuffers() const
	{
		return m_framebuffers;
	}
}