#pragma once
#include "vulkanRenderPass.h"
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class VmaImage;
	class RenderTexture2d;



	/// <summary>
	/// Runs parallel to shadow/forward/postProcessing.
    /// Final result is then overlayed ontop of forwardRenderpass renterTextures.
	/// </summary>
	class GizmoRenderPass : public RenderPass
	{
	private: // Members:
		// These images are transiant (only needed temporary in between pipeline stages).
		// Thus creating texture wrappers for them makes no sense, as they have a fixed place and functionality.
		std::vector<std::unique_ptr<VmaImage>> m_pMsaaImages;
		std::vector<std::unique_ptr<VmaImage>> m_pDepthImages;
		std::vector<std::unique_ptr<RenderTexture2d>> m_pRenderTextures;
		VkFormat m_depthFormat;

	public: // Methods:
		GizmoRenderPass(uint32_t renderWidth, uint32_t renderHeight);
		~GizmoRenderPass();

		// Non-copyable:
		GizmoRenderPass(const GizmoRenderPass&) = delete;
		GizmoRenderPass& operator=(const GizmoRenderPass&) = delete;

		// Movable:
		GizmoRenderPass(GizmoRenderPass&& other) noexcept = default;
		GizmoRenderPass& operator=(GizmoRenderPass&& other) noexcept = default;

		// Getters:
		const VmaImage* const GetMsaaVmaImage(uint32_t frameIndex) const;
		const VmaImage* const GetDepthVmaImage(uint32_t frameIndex) const;
		RenderTexture2d* GetRenderTexture(uint32_t frameIndex) const;

	private: // Methods:
		void CreateRenderTextures(uint32_t renderWidth, uint32_t renderHeight);
		void CreateRenderPass();
		void CreateMsaaImages();
		void CreateDepthImages();
		void CreateFrameBuffers();
	};
}