#ifndef __INCLUDE_GUARD_vulkanHelper_h__
#define __INCLUDE_GUARD_vulkanHelper_h__
#include <vulkan/vulkan.h>
#include "vulkanContext.h"
#include "vulkanCommand.h"
#include "vmaBuffer.h"
#include "vmaImage.h"



/// <summary>
/// Helper functionality for Vulkan.
/// </summary>
namespace VulkanHelper
{
	uint32_t FindMemoryType(VulkanContext* context, uint32_t typeFilter, VkMemoryPropertyFlags properties);
	VulkanCommand BeginSingleTimeCommand(VulkanContext* context, const VulkanQueue& queue);
	void EndSingleTimeCommand(VulkanContext* context, const VulkanCommand& command, const VulkanQueue& queue);
	void CopyBufferToImage(VulkanContext* context, VmaBuffer* buffer, VmaImage* image, const VulkanQueue& queue);
};



#endif // __INCLUDE_GUARD_vulkanHelper_h__