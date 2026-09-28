#pragma once
#include <cstdint>
#include <vulkan/vulkan.h>



namespace emberBackendInterface
{
	/// <summary>
	/// Vulkan-specific texture coupling for backends that consume Vulkan image views directly.
	/// </summary>
	class IVulkanTexture
	{
	public: // Methods:
		// Virtual destructor for v-table:
		virtual ~IVulkanTexture() = default;

		// Getters:
		virtual VkImageView GetVkImageView(uint32_t frameIndex) const = 0;
		virtual VkImageLayout GetVkImageLayout(uint32_t frameIndex) const = 0;
	};
}