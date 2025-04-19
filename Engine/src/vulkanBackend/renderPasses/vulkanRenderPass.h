#ifndef __INCLUDE_GUARD_vulkanRenderPass_h__
#define __INCLUDE_GUARD_vulkanRenderPass_h__
#include <vector>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	namespace vulkanBackend
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
			virtual ~RenderPass();
			const VkRenderPass& GetVkRenderPass() const;
			const std::vector<VkFramebuffer>& GetFramebuffers() const;
		};
	}
}



#endif // __INCLUDE_GUARD_vulkanRenderPass_h__