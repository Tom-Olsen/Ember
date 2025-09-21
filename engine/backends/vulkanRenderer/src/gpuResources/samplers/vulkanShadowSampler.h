#pragma once
#include "vulkanSampler.h"
#include <string>



namespace vulkanRendererBackend
{
	class ShadowSampler : public Sampler
	{
	public: // Methods:
		ShadowSampler(const std::string& name);
		~ShadowSampler();

		// Non-copyable:
		ShadowSampler(const ShadowSampler&) = delete;
		ShadowSampler& operator=(const ShadowSampler&) = delete;

		// Movable:
		ShadowSampler(ShadowSampler&& other) noexcept = default;
		ShadowSampler& operator=(ShadowSampler&& other) noexcept = default;
	};
}