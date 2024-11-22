// needs to be defined before including stb_image.h, but may not be in the header file!
#define STB_IMAGE_IMPLEMENTATION 
#include "vulkanSampler.h"
#include "vulkanMacros.h"



// Constructor:
VulkanSampler::VulkanSampler(VulkanContext* context, const std::string& name)
{
	this->context = context;
	this->name = name;
}



// Destructors:
VulkanSampler::~VulkanSampler()
{
	vkDestroySampler(context->LogicalDevice(), sampler, nullptr);
}
VulkanSampler* VulkanSampler::ColorSampler(VulkanContext* context, const std::string& name)
{
	VulkanSampler* sampler = new VulkanSampler(context, name);
	VkPhysicalDeviceProperties properties = sampler->GetDeviceProperties();

	VkSamplerCreateInfo samplerInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
	samplerInfo.magFilter = VK_FILTER_LINEAR;							// magnified texels filter
	samplerInfo.minFilter = VK_FILTER_LINEAR;							// minified texels filter
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;			// texture u coordinates outside the image
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;			// texture v coordinates outside the image
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;			// texture w coordinates outside the image
	samplerInfo.anisotropyEnable = VK_TRUE;								// anisotropic filtering
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;	// lower values = better performance but lower quality
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;			// border color for: addressModeU/V/W = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE/BORDER
	samplerInfo.unnormalizedCoordinates = VK_FALSE;						// VK_FALSE: uvw € [0,1], VK_TRUE: uvw € [0, width/height/depth]
	samplerInfo.compareEnable = VK_FALSE;								// enable comparison against a reference value
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;						// comparison function to apply
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;				// how to handle mipmapping
	samplerInfo.mipLodBias = 0.0f;										// level of detail bias for mipmap level
	samplerInfo.minLod = 0.0f;											// minimum level of detail to pick
	samplerInfo.maxLod = VK_LOD_CLAMP_NONE;								// maximum level of detail to pick. No clamping = maximum mip level supported by the image
	VKA(vkCreateSampler(context->LogicalDevice(), &samplerInfo, nullptr, &sampler->sampler));

	return sampler;
}
VulkanSampler* VulkanSampler::ShadowSampler(VulkanContext* context, const std::string& name)
{
	VulkanSampler* sampler = new VulkanSampler(context, name);
	VkPhysicalDeviceProperties properties = sampler->GetDeviceProperties();

	VkSamplerCreateInfo samplerInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
	samplerInfo.magFilter = VK_FILTER_LINEAR;							// magnified texels filter
	samplerInfo.minFilter = VK_FILTER_LINEAR;							// minified texels filter
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;	// texture u coordinates outside the image
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;	// texture v coordinates outside the image
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;	// texture w coordinates outside the image
	samplerInfo.anisotropyEnable = VK_FALSE;							// no anisotropic filtering for depth mapping
	samplerInfo.maxAnisotropy = 0.0f;									// lower values = better performance but lower quality
	samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;		// border color for: addressModeU/V/W = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE/BORDER
	samplerInfo.unnormalizedCoordinates = VK_FALSE;						// VK_FALSE: uvw € [0,1], VK_TRUE: uvw € [0, width/height/depth]
	samplerInfo.compareEnable = VK_TRUE;								// enable comparison against a reference value
	samplerInfo.compareOp = VK_COMPARE_OP_LESS_OR_EQUAL;				// comparison function to apply
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;				// how to handle mipmapping
	samplerInfo.mipLodBias = 0.0f;										// level of detail bias for mipmap level
	samplerInfo.minLod = 0.0f;											// minimum level of detail to pick
	samplerInfo.maxLod = VK_LOD_CLAMP_NONE;								// maximum level of detail to pick. No clamping = maximum mip level supported by the image
	VKA(vkCreateSampler(context->LogicalDevice(), &samplerInfo, nullptr, &sampler->sampler));

	return sampler;
}



// Public methods:
VkPhysicalDeviceProperties VulkanSampler::GetDeviceProperties()
{
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(context->PhysicalDevice(), &properties);
	return properties;
}