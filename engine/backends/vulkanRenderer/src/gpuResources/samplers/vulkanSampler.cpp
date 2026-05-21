#include "vulkanSampler.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Constructor/Destructors:
	Sampler::Sampler()
	{
		m_sampler = VK_NULL_HANDLE;
	}
	Sampler::~Sampler()
	{
		Cleanup();
	}



	// Move semantics:
	Sampler::Sampler(Sampler&& other) noexcept
	{
		MoveFrom(other);
	}
	Sampler& Sampler::operator=(Sampler&& other) noexcept
	{
		if (this != &other)
		{
			Cleanup();
			MoveFrom(other);
		}
		return *this;
	}



	// Public methods:
	// Getters:
	VkSampler& Sampler::GetVkSampler()
	{
		return m_sampler;
	}



	// Private methods:
	void Sampler::Cleanup()
	{
		vkDestroySampler(Context::GetVkDevice(), m_sampler, nullptr);
	}
	void Sampler::MoveFrom(Sampler& other) noexcept
	{
		m_sampler = other.m_sampler;

		other.m_sampler = VK_NULL_HANDLE;
	}
}