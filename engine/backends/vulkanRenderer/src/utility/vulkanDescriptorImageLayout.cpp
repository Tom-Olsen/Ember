#include "vulkanDescriptorImageLayout.h"
#include <stdexcept>



namespace vulkanRendererBackend
{
	VkImageLayout GetDescriptorImageLayout(VkDescriptorType descriptorType)
	{
		if (descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
			return VK_IMAGE_LAYOUT_GENERAL;
		if (descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE || descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
			return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		throw std::runtime_error("Unsupported image descriptor type.");
	}
}