// needs to be defined before including stb_image.h, but may not be in the header file!
#define STB_IMAGE_IMPLEMENTATION 
#include "vulkanSampler.h"
#include <iostream>
#include "vulkanHelper.h"
#include "macros.h"



// Constructor:
VulkanSampler::VulkanSampler(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice)
{
	this->logicalDevice = logicalDevice;
	this->physicalDevice = physicalDevice;

	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(physicalDevice->device, &properties);

	VkSamplerCreateInfo samplerInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
	samplerInfo.magFilter = VK_FILTER_LINEAR;	// how to filter magnified texels
	samplerInfo.minFilter = VK_FILTER_LINEAR;	// how to filter minified texels
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;	// how to handle texture coordinates outside the image
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;	// how to handle texture coordinates outside the image
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;	// how to handle texture coordinates outside the image
	samplerInfo.anisotropyEnable = VK_TRUE;	// enable anisotropic filtering
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;	// lower values = better performance but lower quality
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;	// border color to use when using clamp to border addressing mode
	samplerInfo.unnormalizedCoordinates = VK_FALSE; // uvw € [0,1] addressing. If true, uvw € [0, width/height/depth]
	samplerInfo.compareEnable = VK_FALSE;	// enable comparison against a reference value
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS; // comparison function to apply
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;	// how to handle mipmapping
	samplerInfo.mipLodBias = 0.0f;	// level of detail bias for mipmap level
	samplerInfo.minLod = 0.0f;				// minimum level of detail to pick
	samplerInfo.maxLod = VK_LOD_CLAMP_NONE;	// maximum level of detail to pick. No clamping means up to the maximum mip level supported by the image
	VKA(vkCreateSampler(logicalDevice->device, &samplerInfo, nullptr, &sampler));
}



// Destructor:
VulkanSampler::~VulkanSampler()
{
	vkDestroySampler(logicalDevice->device, sampler, nullptr);
}