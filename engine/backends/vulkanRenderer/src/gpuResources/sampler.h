#ifndef __INCLUDE_GUARD_sampler_h__
#define __INCLUDE_GUARD_sampler_h__
#include <string>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	class Sampler
	{
	private: // Members:
		VkSampler m_sampler;
		std::string m_name;

	private: // Methods:
		Sampler(const std::string& name);

	public: // Methods:
		~Sampler();

		// Non-copyable:
		Sampler(const Sampler&) = delete;
		Sampler& operator=(const Sampler&) = delete;

		// Movable:
		Sampler(Sampler&& other) noexcept;
		Sampler& operator=(Sampler&& other) noexcept;

		// Getters:
		VkSampler& GetVkSampler();
		const std::string& GetName() const;

		VkPhysicalDeviceProperties GetVkPhysicalDeviceProperties() const;

		// Static specialised constructors:
		static Sampler* ColorSampler(const std::string& name);
		static Sampler* ShadowSampler(const std::string& name);

	private: // Methods:
		void Cleanup();
		void MoveFrom(Sampler& other) noexcept;
	};
}



#endif // __INCLUDE_GUARD_sampler_h__