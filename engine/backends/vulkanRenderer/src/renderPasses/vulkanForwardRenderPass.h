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
	/// Basic forward render pass.
	/// </summary>
	class ForwardRenderPass : public RenderPass
	{
	private: // Members:
		// These images are transiant (only needed temporary in between pipeline stages).
		// Thus creating texture wrappers for them makes no sense, as they have a fixed place and functionality.
		std::vector<std::unique_ptr<VmaImage>> m_pMsaaImages;
		std::vector<std::unique_ptr<VmaImage>> m_pDepthImages;
		std::vector<std::unique_ptr<RenderTexture2d>> m_pRenderTextures;
		std::vector<std::unique_ptr<RenderTexture2d>> m_pSecondaryRenderTextures;
		VkFormat m_depthFormat;

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
		const VmaImage* const GetMsaaVmaImage(uint32_t frameIndex) const;
		const VmaImage* const GetDepthVmaImage(uint32_t frameIndex) const;
		RenderTexture2d* GetRenderTexture(uint32_t frameIndex) const;
		RenderTexture2d* GetSecondaryRenderTexture(uint32_t frameIndex) const;

	private: // Methods:
		void CreateRenderPass();
		void CreateRenderTextures(uint32_t renderWidth, uint32_t renderHeight);
		void CreateMsaaImages();
		void CreateDepthImages();
		void CreateFrameBuffers();
	};
}