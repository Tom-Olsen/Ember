#ifndef __INCLUDE_GUARD_sampler_h__
#define __INCLUDE_GUARD_sampler_h__
#include <vulkan/vulkan.h>
#include <string>



namespace emberEngine
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

		// Getters:
		VkSampler& GetVkSampler();
		const std::string& GetName() const;

		VkPhysicalDeviceProperties GetVkPhysicalDeviceProperties() const;

		// Static specialised constructors:
		static Sampler* ColorSampler(const std::string& name);
		static Sampler* ShadowSampler(const std::string& name);
	};
}



#endif // __INCLUDE_GUARD_sampler_h__