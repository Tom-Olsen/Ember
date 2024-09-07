#ifndef __INCLUDE_GUARD_vulkanIndexBuffer_h__
#define __INCLUDE_GUARD_vulkanIndexBuffer_h__
#include <vulkan/vulkan.h>
#include <memory>
#include "vulkanBuffer.h"
#include "vulkanCommands.h"
#include "mesh.h"



class VulkanIndexBuffer
{
public: // Members:
	std::unique_ptr<VulkanBuffer> buffer;

public: // Methods:
	VulkanIndexBuffer(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, Mesh* mesh);
	~VulkanIndexBuffer();

	void UpdateBuffer(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, Mesh* mesh);
};



#endif // __INCLUDE_GUARD_vulkanIndexBuffer_h__