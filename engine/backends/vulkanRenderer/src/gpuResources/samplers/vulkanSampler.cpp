#include "vulkanSampler.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"



namespace vulkanRendererBackend
{
	// Constructor/Destructors:
	Sampler::Sampler()
	{
		m_sampler = VK_NULL_HANDLE;
		m_name = "";
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
	const std::string& Sampler::GetName() const
	{
		return m_name;
	}
	VkPhysicalDeviceProperties Sampler::GetVkPhysicalDeviceProperties() const
	{
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(Context::GetVkPhysicalDevice(), &properties);
		return properties;
	}



	// Private methods:
	void Sampler::Cleanup()
	{
		vkDestroySampler(Context::GetVkDevice(), m_sampler, nullptr);
	}
	void Sampler::MoveFrom(Sampler& other) noexcept
	{
		m_sampler = other.m_sampler;
		m_name = other.m_name;

		other.m_sampler = VK_NULL_HANDLE;
		other.m_name = "";
	}
}