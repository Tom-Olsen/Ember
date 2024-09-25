#include "vulkanHelper.h"
#include <iostream>
#include "macros.h"



namespace VulkanHelper
{
	uint32_t FindMemoryType(VulkanPhysicalDevice* physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		// Memory heaps are distinct memory resources like dedicated VRAM and swap space in RAM for when VRAM runs out.
		// The different types of memory exist within these heaps.
		// For now I will only concern myself with the type of memory and not the heap it comes from.

		VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice->device, &deviceMemoryProperties);

		for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++)
		{
			// Check if required memory type is allowed
			if (typeFilter & (1 << i))
				// Check if at least the required properties are satisfied
				if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
					return i;
		}

		throw std::runtime_error("Failed to find suitable memory type!");
		return -1;
	}

	VulkanCommands BeginSingleTimeCommands(VulkanLogicalDevice* logicalDevice, const VulkanQueue& queue)
	{
		VulkanCommands commands(1, logicalDevice, queue);
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VKA(vkBeginCommandBuffer(commands.buffers[0], &beginInfo));
		return commands;
	}

	void EndSingleTimeCommands(VulkanLogicalDevice* logicalDevice, const VulkanCommands& commands, const VulkanQueue& queue)
	{
		vkEndCommandBuffer(commands.buffers[0]);
		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commands.buffers[0];
		VKA(vkQueueSubmit(queue.queue, 1, &submitInfo, VK_NULL_HANDLE));
		VKA(vkQueueWaitIdle(queue.queue));
	}

	void CopyBufferToImage(VulkanLogicalDevice* logicalDevice, VulkanBuffer* buffer, VulkanImage* image, const VulkanQueue& queue)
	{
		VulkanCommands commands = BeginSingleTimeCommands(logicalDevice, queue);

		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { static_cast<uint32_t>(image->width) , static_cast<uint32_t>(image->height), static_cast<uint32_t>(image->depth) };

		vkCmdCopyBufferToImage(
			commands.buffers[0],
			buffer->buffer,
			image->image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region);

		EndSingleTimeCommands(logicalDevice, commands, queue);
	}
}