#include "vulkanRenderPass.h"
#include "vulkanContext.h"
#include <stdexcept>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
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



	// Getters:
	const VkRenderPass& RenderPass::GetVkRenderPass() const
	{
		return m_renderPass;
	}
	const VkFramebuffer& RenderPass::GetFramebuffer(int index) const
	{
		if (index < 0 || static_cast<size_t>(index) >= m_framebuffers.size())
			throw std::out_of_range("RenderPass::GetFramebuffer(...) failed. Index out of range.");
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
			if (m_framebuffers[i] != nullptr)
				vkDestroyFramebuffer(Context::GetVkDevice(), m_framebuffers[i], nullptr);
		m_framebuffers.clear();

		// Destroy renderpass:
		if (m_renderPass != nullptr)
			vkDestroyRenderPass(Context::GetVkDevice(), m_renderPass, nullptr);
		m_renderPass = VK_NULL_HANDLE;
	}
	void RenderPass::MoveFrom(RenderPass& other) noexcept
	{
		m_renderPass = other.m_renderPass;
		m_framebuffers = std::move(other.m_framebuffers);

		other.m_renderPass = VK_NULL_HANDLE;
		other.m_framebuffers.clear();
	}
}