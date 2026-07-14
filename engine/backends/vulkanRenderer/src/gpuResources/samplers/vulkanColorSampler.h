#pragma once
#include "vulkanSampler.h"
#include <string>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	class ColorSampler : public Sampler
	{
	    // Color sampler naming:
		// colorSampler is the recommended default sampler.
		// Additional color samplers start from colorSampler and append only the settings that deviate from it.
		// Examples:
		// colorSamplerClampEdge
		// colorSamplerClampBorder
		// colorSamplerMirror
		// colorSamplerNearest
		// colorSamplerCubicClampEdge
		// colorSamplerMipmapNearest
		// colorSamplerNearestClampEdgeMipmapNearest

	public: // Types:
		struct Settings
		{
            std::string name = "Sampler_Color";
			VkFilter filter = VK_FILTER_LINEAR;
			VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		};

	public: // Methods:
		ColorSampler(Settings settings);
		~ColorSampler();

		// Non-copyable:
		ColorSampler(const ColorSampler&) = delete;
		ColorSampler& operator=(const ColorSampler&) = delete;

		// Movable:
		ColorSampler(ColorSampler&& other) noexcept = default;
		ColorSampler& operator=(ColorSampler&& other) noexcept = default;
	};
}