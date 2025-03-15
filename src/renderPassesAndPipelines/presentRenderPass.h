#ifndef __INCLUDE_GUARD_presentRenderpass_h__
#define __INCLUDE_GUARD_presentRenderpass_h__
#include "renderPass.h"
#include <memory>
#include <vulkan/vulkan.h>



namespace emberEngine
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



#endif // __INCLUDE_GUARD_presentRenderpass_h__