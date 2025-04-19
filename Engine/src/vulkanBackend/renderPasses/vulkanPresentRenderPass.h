#ifndef __INCLUDE_GUARD_vulkanPresentRenderpass_h__
#define __INCLUDE_GUARD_vulkanPresentRenderpass_h__
#include "vulkanRenderPass.h"
#include <memory>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	namespace vulkanBackend
	{
		/// <summary>
		/// Basic forward render pass.
		/// </summary>
		class PresentRenderPass : public RenderPass
		{
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