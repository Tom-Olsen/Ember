#ifndef __INCLUDE_GUARD_vulkanHelper_h__
#define __INCLUDE_GUARD_vulkanHelper_h__
#include <vulkan/vulkan.h>
#include "vulkanPhysicalDevice.h"
#include "vulkanLogicalDevice.h"
#include "vulkanCommands.h"
#include "vulkanBuffer.h"
#include "vulkanImage.h"



/// <summary>
/// Helper functionality for Vulkan.
/// </summary>
namespace VulkanHelper
{
	uint32_t FindMemoryType(VulkanPhysicalDevice* physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
	VulkanCommands BeginSingleTimeCommands(VulkanLogicalDevice* logicalDevice, const VulkanQueue& queue);
	void EndSingleTimeCommands(VulkanLogicalDevice* logicalDevice, const VulkanCommands& commands, const VulkanQueue& queue);
	void CopyBufferToImage(VulkanLogicalDevice* logicalDevice, VulkanBuffer* buffer, VulkanImage* image, const VulkanQueue& queue);
};



#endif // __INCLUDE_GUARD_vulkanHelper_h__