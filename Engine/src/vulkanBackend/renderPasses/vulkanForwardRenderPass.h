#ifndef __INCLUDE_GUARD_vulkanForwardRenderpass_h__
#define __INCLUDE_GUARD_vulkanForwardRenderpass_h__
#include "vulkanRenderPass.h"
#include <memory>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	// Forward declarations:
	class RenderTexture2d;



	namespace vulkanBackend
	{
		// Forward declarations:
		class VmaImage;



		/// <summary>
		/// Basic forward render pass.
		/// </summary>
		class ForwardRenderPass : public RenderPass
		{
		private: // Members:
			// These images are transiant (only needed temporary in between pipeline stages).
			// Thus creating texture wrappers for them makes no sense, as they have a fixed place and functionality.
			std::unique_ptr<VmaImage> m_pMsaaImage;
			std::unique_ptr<VmaImage> m_pDepthImage;
			std::unique_ptr<RenderTexture2d> m_pRenderTexture;
			std::unique_ptr<RenderTexture2d> m_pSecondaryRenderTexture;
			VkFormat m_depthFormat;

		public: // Methods:
			ForwardRenderPass(uint32_t renderWidth, uint32_t renderHeight);
			~ForwardRenderPass();
			const VmaImage* const GetMsaaVmaImage() const;
			const VmaImage* const GetDepthVmaImage() const;
			RenderTexture2d* GetRenderTexture() const;
			RenderTexture2d* GetSecondaryRenderTexture() const;

		private: // Methods:
			void CreateRenderPass();
			void CreateMsaaImage();
			void CreateDepthImage();
			void CreateFrameBuffers();
		};
	}
}



#endif // __INCLUDE_GUARD_vulkanForwardRenderpass_h__