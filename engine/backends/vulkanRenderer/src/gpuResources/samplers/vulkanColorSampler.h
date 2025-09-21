#pragma once
#include "vulkanSampler.h"
#include <string>



namespace vulkanRendererBackend
{
	class ColorSampler : public Sampler
	{
	public: // Methods:
		ColorSampler(const std::string& name);
		~ColorSampler();

		// Non-copyable:
		ColorSampler(const ColorSampler&) = delete;
		ColorSampler& operator=(const ColorSampler&) = delete;

		// Movable:
		ColorSampler(ColorSampler&& other) noexcept = default;
		ColorSampler& operator=(ColorSampler&& other) noexcept = default;
	};
}