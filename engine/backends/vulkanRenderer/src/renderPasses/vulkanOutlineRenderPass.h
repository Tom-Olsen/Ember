#pragma once
#include "vulkanRenderPass.h"
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class RenderTexture2d;



	/// <summary>
	/// Runs parallel to shadow/forward.
	/// Renders a binary mask into its own framebuffer.
    /// Post-render compute derives and composites a hollow outline directly onto the forward image before gizmos.
	/// </summary>
	class OutlineRenderPass : public RenderPass
	{
	private: // Members:
		std::vector<std::unique_ptr<RenderTexture2d>> m_pRenderTextures;

	public: // Methods:
		OutlineRenderPass(uint32_t renderWidth, uint32_t renderHeight);
		~OutlineRenderPass();

		// Non-copyable:
		OutlineRenderPass(const OutlineRenderPass&) = delete;
		OutlineRenderPass& operator=(const OutlineRenderPass&) = delete;

		// Movable:
		OutlineRenderPass(OutlineRenderPass&& other) noexcept = default;
		OutlineRenderPass& operator=(OutlineRenderPass&& other) noexcept = default;

		// Getters:
		RenderTexture2d* GetRenderTexture(uint32_t frameIndex) const;

	private: // Methods:
		void CreateRenderTextures(uint32_t renderWidth, uint32_t renderHeight);
		void CreateRenderPass();
		void CreateFrameBuffers();
	};
}