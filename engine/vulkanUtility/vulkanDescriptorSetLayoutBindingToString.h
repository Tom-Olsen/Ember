#pragma once
#include "vulkanDescriptorTypeToString.h"
#include "vulkanShaderStageFlagsToString.h"
#include <sstream>
#include <vulkan/vulkan.h>



namespace emberVulkanUtility
{
	inline std::string ToString(VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding)
	{
		std::ostringstream ss;
		ss << "binding: " << vkDescriptorSetLayoutBinding.binding << ", descriptorType: " << emberVulkanUtility::ToString(vkDescriptorSetLayoutBinding.descriptorType) << ", ";
		ss << "descriptorCount: " << vkDescriptorSetLayoutBinding.descriptorCount << ", shaderStage: " << emberVulkanUtility::ToString_VkShaderStageFlags(vkDescriptorSetLayoutBinding.stageFlags) << ", ";
		ss << "sampler: " << vkDescriptorSetLayoutBinding.pImmutableSamplers;
		return ss.str();
	}
}