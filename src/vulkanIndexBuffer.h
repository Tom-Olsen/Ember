#ifndef __INCLUDE_GUARD_vulkanIndexBuffer_h__
#define __INCLUDE_GUARD_vulkanIndexBuffer_h__
#include <vulkan/vulkan.h>
#include <memory>
#include "vulkanBuffer.h"
#include "vulkanCommand.h"
#include "mesh.h"



/// <summary>
/// VulkanIndexBuffer class owns index buffer.
/// TODO: merge index and vertex buffers into one class with shared buffer and appropriate offsets.
/// </summary>
class VulkanIndexBuffer
{
public: // Members:
	std::unique_ptr<VulkanBuffer> buffer;

private: // Members:
	VulkanLogicalDevice* logicalDevice;
	VulkanPhysicalDevice* physicalDevice;

public: // Methods:
	VulkanIndexBuffer(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, Mesh* mesh);
	~VulkanIndexBuffer();
	void UpdateBuffer(Mesh* mesh);
};



#endif // __INCLUDE_GUARD_vulkanIndexBuffer_h__