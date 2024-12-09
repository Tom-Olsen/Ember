#ifndef __INCLUDE_GUARD_sampler_h__
#define __INCLUDE_GUARD_sampler_h__
#include <vulkan/vulkan.h>
#include <string>



struct VulkanContext;



class Sampler
{
private: // Members:
	VkSampler m_sampler;
	std::string m_name;
	VulkanContext* m_pContext;

private: // Methods:
	Sampler(VulkanContext* pContext, const std::string& name);
public: // Methods:
	~Sampler();

	// Getters:
	VkSampler& GetVkSampler();
	const std::string& GetName() const;

	VkPhysicalDeviceProperties GetVkPhysicalDeviceProperties() const;

	// Static specialised constructors:
	static Sampler* ColorSampler(VulkanContext* pContext, const std::string& name);
	static Sampler* ShadowSampler(VulkanContext* pContext, const std::string& name);
};



#endif // __INCLUDE_GUARD_sampler_h__