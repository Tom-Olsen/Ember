#include "vulkanRenderPass.h"
#include "vulkanContext.h"



namespace vulkanRendererBackend
{
	// Virtual Destructor:
	RenderPass::~RenderPass()
	{
		Cleanup();
	}



	// Move semantics:
	RenderPass::RenderPass(RenderPass&& other) noexcept
	{
		MoveFrom(other);
	}
	RenderPass& RenderPass::operator=(RenderPass&& other) noexcept
	{
		if (this != &other)
		{
			Cleanup();
			MoveFrom(other);
		}
		return *this;
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



	// Private methods:
	void RenderPass::Cleanup()
	{
		// Destroy framebuffers:
		for (uint32_t i = 0; i < m_framebuffers.size(); i++)
			vkDestroyFramebuffer(Context::GetVkDevice(), m_framebuffers[i], nullptr);
		m_framebuffers.clear();

		// Destroy renderpass:
		vkDestroyRenderPass(Context::GetVkDevice(), m_renderPass, nullptr);
	}
	void RenderPass::MoveFrom(RenderPass& other) noexcept
	{
		m_renderPass = other.m_renderPass;
		m_framebuffers = std::move(other.m_framebuffers);

		other.m_renderPass = VK_NULL_HANDLE;
		other.m_framebuffers.clear();
	}
}