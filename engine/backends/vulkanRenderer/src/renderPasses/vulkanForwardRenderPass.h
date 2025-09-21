#pragma once
#include "vulkanRenderPass.h"
#include "vulkanFormat.h"
#include <memory>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class VmaImage;
	class RenderTexture2d;



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
		Format m_depthFormat;

	public: // Methods:
		ForwardRenderPass(uint32_t renderWidth, uint32_t renderHeight);
		~ForwardRenderPass();

		// Non-copyable:
		ForwardRenderPass(const ForwardRenderPass&) = delete;
		ForwardRenderPass& operator=(const ForwardRenderPass&) = delete;

		// Movable:
		ForwardRenderPass(ForwardRenderPass&& other) noexcept = default;
		ForwardRenderPass& operator=(ForwardRenderPass&& other) noexcept = default;

		// Getters:
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