#ifndef __INCLUDE_GUARD_vulkanRendererBackend_vulkanPresentRenderpass_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_vulkanPresentRenderpass_h__
#include "vulkanRenderPass.h"
#include <memory>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	/// <summary>
	/// Renders final renderTexture of the forward render pass to the swap.
	/// </summary>
	class PresentRenderPass : public RenderPass
	{
	public: // Methods:
		PresentRenderPass();
		~PresentRenderPass();

		// Non-copyable:
		PresentRenderPass(const PresentRenderPass&) = delete;
		PresentRenderPass& operator=(const PresentRenderPass&) = delete;

		// Movable:
		PresentRenderPass(PresentRenderPass&& other) noexcept = default;
		PresentRenderPass& operator=(PresentRenderPass&& other) noexcept = default;

	private: // Methods:
		void CreateRenderPass();
		void CreateFrameBuffers();
	};
}



#endif // __INCLUDE_GUARD_vulkanRendererBackend_vulkanPresentRenderpass_h__