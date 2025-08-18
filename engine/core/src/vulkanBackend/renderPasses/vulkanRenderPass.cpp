#include "vulkanRenderPass.h"
#include "vulkanContext.h"



namespace emberEngine
{
	namespace vulkanBackend
	{
		// Virtual Destructor:
		RenderPass::~RenderPass()
		{
			// Destroy framebuffers:
			for (uint32_t i = 0; i < m_framebuffers.size(); i++)
				vkDestroyFramebuffer(Context::GetVkDevice(), m_framebuffers[i], nullptr);
			m_framebuffers.clear();

			// Destroy renderpass:
			vkDestroyRenderPass(Context::GetVkDevice(), m_renderPass, nullptr);
		}



		// Public methods:
		const VkRenderPass& RenderPass::GetVkRenderPass() const
		{
			return m_renderPass;
		}
		const VkFramebuffer& RenderPass::GetFramebuffer(int index) const
		{
			return m_framebuffers[index];
		}
		const std::vector<VkFramebuffer>& RenderPass::GetFramebuffers() const
		{
			return m_framebuffers;
		}
	}
}