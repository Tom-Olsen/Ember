#ifndef __INCLUDE_GUARD_vulkanVertexBuffer_h__
#define __INCLUDE_GUARD_vulkanVertexBuffer_h__
#include <vulkan/vulkan.h>
#include <memory>
#include "vulkanBuffer.h"
#include "vulkanCommands.h"
#include "mesh.h"



/// <summary>
/// VulkanVertexBuffer class owns vertex buffer.
/// One buffer for all vertex data responsible.
/// The vertex data is stored as: allPositions | allColors | ... instead of interleaved.
/// TODO: merge index and vertex buffers into one class with shared buffer and appropriate offsets.
/// </summary>
class VulkanVertexBuffer
{
private: // Members:
	VulkanLogicalDevice* logicalDevice;
	VulkanPhysicalDevice* physicalDevice;

public: // Members:
	std::unique_ptr<VulkanBuffer> buffer;

public: // Methods:
	VulkanVertexBuffer(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, Mesh* mesh);
	~VulkanVertexBuffer();
	void UpdateBuffer(Mesh* mesh);
};



#endif // __INCLUDE_GUARD_vulkanBuffer_h__