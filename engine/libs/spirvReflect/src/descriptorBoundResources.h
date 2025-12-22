#pragma once
#include "uniformBufferBlock.h"
#include <string>
#include <vector>
#include <vulkan/vulkan.h>



namespace emberSpirvReflect
{
	// Reflects all resources bound to a shader.
	struct DescriptorBoundResources
	{
	public: // Members:
		uint32_t bindingCount = 0; // number of descriptorSetBindingNames and descriptorSetLayoutBindings
		std::vector<std::string> descriptorSetBindingNames;
		std::vector<VkDescriptorSetLayoutBinding> vkDescriptorSetLayoutBindings;
		std::unordered_map<std::string, UniformBufferBlock> uniformBufferBlockMap;	// VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER.
		std::unordered_map<std::string, VkImageViewType> sampleViewTypeMap;			// VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE.
		std::unordered_map<std::string, VkImageViewType> storageViewTypeMap;		// VK_DESCRIPTOR_TYPE_STORAGE_IMAGE.

	public: // Methods:
		// Constructor:
		// Move initialization here.
		
		// Getters:
		// GetCound? and remove bindingCount member? bindingCount = descriptorSetBindingNames.size()?

		// Debugging:
		std::string ToString() const;
	};
}