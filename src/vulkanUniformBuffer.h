#pragma once
#ifndef __INCLUDE_GUARD_vulkanUniformBuffer_h__
#define __INCLUDE_GUARD_vulkanUniformBuffer_h__
#include <vulkan/vulkan.h>
#include <memory>
#include "vulkanContext.h"
#include "vmaBuffer.h"
#include "vulkanCommand.h"
#include "glmTypes.h"
#include "uniformObject.h"



class VulkanUniformBuffer
{
public: // Members:
	uint64_t size;	// in bytes
	std::shared_ptr<VmaBuffer> buffer; // TODO: using unique ptr here causes a crash, why?

private: // Members:
	VulkanContext* context;
	void* data;

public: // Methods:
	VulkanUniformBuffer(VulkanContext* context, uint64_t size);
	~VulkanUniformBuffer();
	template<typename T>
	void UpdateBuffer(const T& datastruct);
};



#endif // __INCLUDE_GUARD_vulkanUniformBuffer_h__