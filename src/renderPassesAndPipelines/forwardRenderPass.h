#ifndef __INCLUDE_GUARD_forwardRenderpass_h__
#define __INCLUDE_GUARD_forwardRenderpass_h__
#include "renderPass.h"
#include <memory>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	// Forward declarations:
	class VmaImage;
	struct VulkanContext;



	/// <summary>
	/// Basic forward render pass.
	/// </summary>
	class ForwardRenderPass : public RenderPass
	{
	private: // Members:
		// These images are transiant (only needed temporary in between pipeline stages).
		// Thus creating texture wrappers for them makes no sense, as they have a fixed place and functionality.
		std::unique_ptr<VmaImage> m_msaaImage;
		std::unique_ptr<VmaImage> m_depthImage;
		VkFormat m_depthFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;

	public: // Methods:
		ForwardRenderPass(VulkanContext* pContext);
		~ForwardRenderPass();
		const VmaImage* const GetMsaaVmaImage() const;
		const VmaImage* const GetDepthVmaImage() const;

	private: // Methods:
		void CreateRenderPass();
		void CreateMsaaImage();
		void CreateDepthImage();
		void CreateFrameBuffers();
	};
}



#endif // __INCLUDE_GUARD_forwardRenderpass_h__