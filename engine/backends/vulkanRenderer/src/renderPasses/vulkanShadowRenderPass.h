#pragma once
#include "vulkanRenderPass.h"
#include <memory>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class VmaImage;
	class DepthTexture2dArray;



	/// <summary>
	/// Shadow render pass.
	/// One layer for eacg light source.
	/// </summary>
	class ShadowRenderPass : public RenderPass
	{
	private: // Members:
		std::unique_ptr<DepthTexture2dArray> m_shadowMaps;

	public: // Members:
		static VkFormat s_shadowMapFormat;
		static uint32_t s_layerCount;

	public: // Methods:
		ShadowRenderPass();
		~ShadowRenderPass();

		// Non-copyable:
		ShadowRenderPass(const ShadowRenderPass&) = delete;
		ShadowRenderPass& operator=(const ShadowRenderPass&) = delete;

		// Movable:
		ShadowRenderPass(ShadowRenderPass&& other) noexcept = default;
		ShadowRenderPass& operator=(ShadowRenderPass&& other) noexcept = default;

		// Getters:
		DepthTexture2dArray* const GetShadowMaps() const;

	private: // Methods:
		void CreateRenderpass();
		void CreateFramebuffers();
	};
}