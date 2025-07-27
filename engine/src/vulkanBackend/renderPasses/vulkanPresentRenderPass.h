#ifndef __INCLUDE_GUARD_vulkanPresentRenderpass_h__
#define __INCLUDE_GUARD_vulkanPresentRenderpass_h__
#include "vulkanRenderPass.h"
#include <memory>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	// Forward declarations:
	class RenderTexture2d;



	namespace vulkanBackend
	{
		/// <summary>
		/// Renders final renderTexture of the forward render pass to the swap.
		/// </summary>
		class PresentRenderPass : public RenderPass
		{
		private: // Members:
			int m_renderWidth;
			int m_renderHeight;
			std::unique_ptr<RenderTexture2d> m_pRenderTexture;	// for fake swapchain support.

		public: // Methods:
			PresentRenderPass();
			~PresentRenderPass();

		private: // Methods:
			void CreateRenderPass();
			void CreateFrameBuffers();
		};
	}
}



#endif // __INCLUDE_GUARD_vulkanPresentRenderpass_h__