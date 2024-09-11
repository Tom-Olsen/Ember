#pragma once
#ifndef __INCLUDE_GUARD_vulkanUniformBuffer_h__
#define __INCLUDE_GUARD_vulkanUniformBuffer_h__
#include <vulkan/vulkan.h>
#include <memory>
#include "vulkanBuffer.h"
#include "vulkanCommands.h"
#include "glmTypes.h"



// Temporary struct. Will be replaced later by camera and gameObjects:
struct MatrixData
{
	alignas(16) Float4x4 model;
	alignas(16) Float4x4 view;
	alignas(16) Float4x4 proj;
};
// Vulkan expects the data in structures to be aligned in memory in a specific way, for example:
// Scalars have to be aligned by N (4 bytes given 32 bit floats).
// A vec2 must be aligned by 2N (8 bytes)
// A vec3 or vec4 must be aligned by 4N (16 bytes)
// A nested structure must be aligned by the base alignment of its members rounded up to a multiple of 16.
// A mat4 matrix must have the same alignment as a vec4.
// Full list of alignment requirements in: https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap15.html#interfaces-resources-layout
// 15.8.4. Offset and Stride Assignment / Alignment Requirements



class VulkanUniformBuffer
{
public: // Members:
	uint64_t bufferSize;	// in bytes
	std::shared_ptr<VulkanBuffer> buffer;

private: // Members:
	VulkanLogicalDevice* logicalDevice;
	VulkanPhysicalDevice* physicalDevice;
	void* data;

public: // Methods:
	VulkanUniformBuffer(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, uint64_t bufferSize);
	~VulkanUniformBuffer();
	void UpdateBuffer(const MatrixData& matrixData);
};



#endif // __INCLUDE_GUARD_vulkanUniformBuffer_h__