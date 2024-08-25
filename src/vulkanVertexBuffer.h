#ifndef __INCLUDE_GUARD_vulkanVertexBuffer_h__
#define __INCLUDE_GUARD_vulkanVertexBuffer_h__
#include <vulkan/vulkan.h>
#include <memory>
#include "vulkanBuffer.h"
#include "vulkanCommands.h"
#include "mesh.h"



class VulkanVertexBuffer
{
public: // Members:
	std::unique_ptr<VulkanBuffer> buffer;

public: // Methods:
	VulkanVertexBuffer(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, Mesh* mesh);
	~VulkanVertexBuffer();
};



#endif // __INCLUDE_GUARD_vulkanBuffer_h__