#pragma once
#ifndef __INCLUDE_GUARD_vulkanUniformBuffer_h__
#define __INCLUDE_GUARD_vulkanUniformBuffer_h__
#include <vulkan/vulkan.h>
#include <memory>
#include "vulkanBuffer.h"
#include "vulkanCommand.h"
#include "glmTypes.h"
#include "uniformObject.h"



class VulkanUniformBuffer
{
public: // Members:
	uint64_t bufferSize;	// in bytes
	std::shared_ptr<VulkanBuffer> buffer; // TODO: using unique ptr here causes a crash, why?

private: // Members:
	VulkanLogicalDevice* logicalDevice;
	VulkanPhysicalDevice* physicalDevice;
	void* data;

public: // Methods:
	VulkanUniformBuffer(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, uint64_t bufferSize);
	~VulkanUniformBuffer();
	template<typename T>
	void UpdateBuffer(const T& datastruct);
};



#endif // __INCLUDE_GUARD_vulkanUniformBuffer_h__