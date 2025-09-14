#pragma once
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	/// <summary>
	/// Polymorphic parent class for all renderPasses.
	/// </summary>
	class RenderPass
	{
	protected: // Members:
		VkRenderPass m_renderPass;
		std::vector<VkFramebuffer> m_framebuffers;

	public: // Methods:
		RenderPass() = default;
		virtual ~RenderPass();

		// Non-copyable:
		RenderPass(const RenderPass&) = delete;
		RenderPass& operator=(const RenderPass&) = delete;

		// Movable:
		RenderPass(RenderPass&& other) noexcept;
		RenderPass& operator=(RenderPass&& other) noexcept;

		const VkRenderPass& GetVkRenderPass() const;
		const VkFramebuffer& GetFramebuffer(int index) const;
		const std::vector<VkFramebuffer>& GetFramebuffers() const;

	private: // Methods;
		void Cleanup();
		void MoveFrom(RenderPass& other) noexcept;
	};
}