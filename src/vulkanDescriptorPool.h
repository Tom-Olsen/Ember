#pragma once
#ifndef __INCLUDE_GUARD_vulkanDescriptorPool_h__
#define __INCLUDE_GUARD_vulkanDescriptorPool_h__
#include <vulkan/vulkan.h>
#include "vulkanLogicalDevice.h"
#include "vulkanPipelineLayout.h"
#include "vulkanUniformBuffer.h"
#include "texture2d.h"
#include "memory"



class VulkanDescriptorPool
{
public: // Members:
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

private: // Members:
	uint32_t descriptorCount;
	VulkanLogicalDevice* logicalDevice;
	VulkanPipelineLayout* pipelineLayout;

public: // Methods:
	VulkanDescriptorPool(VulkanLogicalDevice* logicalDevice, VulkanPipelineLayout* pipelineLayout, const std::vector<VulkanUniformBuffer>& uniformBuffers, Texture2d* texture2d, size_t framesInFlight);
	~VulkanDescriptorPool();

private: // Methods:
	void CreateDescriptorPool();
	void CreateDescriptorSets();
	void FillDescriptorSets(const std::vector<VulkanUniformBuffer>& uniformBuffers, Texture2d* texture2d);
};



#endif // __INCLUDE_GUARD_vulkanDescriptorPool_h__