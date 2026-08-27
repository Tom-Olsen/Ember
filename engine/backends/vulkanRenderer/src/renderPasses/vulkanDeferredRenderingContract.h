#pragma once
#include "deferredRenderingConstants.h"
#include <cstdint>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	namespace deferredRenderingContract
	{
		// G-buffer attachment formats:
		inline constexpr VkFormat albedoFormat = VK_FORMAT_R8G8B8A8_SRGB;
		inline constexpr VkFormat normalFormat = VK_FORMAT_A2B10G10R10_UNORM_PACK32;	// rgb = world-space normal, a = unused.
		inline constexpr VkFormat materialFormat = VK_FORMAT_R8G8B8A8_UNORM;			// r = metallic, g = roughness, b = ambient occlusion, a = flags
		inline constexpr VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;

		// Lighting output format:
		inline constexpr VkFormat sceneColorFormat = VK_FORMAT_R16G16B16A16_SFLOAT;

		// G-buffer attachment indices:
		inline constexpr uint32_t albedoAttachmentIndex = 0;
		inline constexpr uint32_t normalAttachmentIndex = 1;
		inline constexpr uint32_t materialAttachmentIndex = 2;
		inline constexpr uint32_t depthAttachmentIndex = 3;
		inline constexpr uint32_t colorAttachmentCount = 3;
		inline constexpr uint32_t attachmentCount = 4;
	}	
}