#ifndef __INCLUDE_GUARD_sampler_h__
#define __INCLUDE_GUARD_sampler_h__
#include <vulkan/vulkan.h>
#include <string>



struct VulkanContext;



class Sampler
{
public: // Members:
	VkSampler sampler;
	std::string name;

private: // Members:
	VulkanContext* context;

public: // Methods:
	~Sampler();
	VkPhysicalDeviceProperties GetDeviceProperties();

	// Static specialised constructors:
	static Sampler* ColorSampler(VulkanContext* context, const std::string& name);
	static Sampler* ShadowSampler(VulkanContext* context, const std::string& name);

private: // Methods:
	Sampler(VulkanContext* context, const std::string& name);
};



#endif // __INCLUDE_GUARD_sampler_h__