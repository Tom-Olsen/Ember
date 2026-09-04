#pragma once
#include "vulkanRenderPass.h"
#include <memory>



namespace vulkanRendererBackend
{
	/// <summary>
	/// Renders the final scene-color texture to the swapchain image.
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