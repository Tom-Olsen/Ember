#include "vulkanHelper.h"
#include <iostream>
#include "macros.h"



namespace VulkanHelper
{
	uint32_t FindMemoryType(VulkanContext* context, uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		// Memory heaps are distinct memory resources like dedicated VRAM and swap space in RAM for when VRAM runs out.
		// The different types of memory exist within these heaps.
		// For now I will only concern myself with the type of memory and not the heap it comes from.

		VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
		vkGetPhysicalDeviceMemoryProperties(context->PhysicalDevice(), &deviceMemoryProperties);

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

	VulkanCommand BeginSingleTimeCommand(VulkanContext* context, const VulkanQueue& queue)
	{
		VulkanCommand command(context, queue);
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VKA(vkBeginCommandBuffer(command.buffer, &beginInfo));
		return command;
	}

	void EndSingleTimeCommand(VulkanContext* context, const VulkanCommand& command, const VulkanQueue& queue)
	{
		vkEndCommandBuffer(command.buffer);
		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &command.buffer;
		VKA(vkQueueSubmit(queue.queue, 1, &submitInfo, VK_NULL_HANDLE));
		VKA(vkQueueWaitIdle(queue.queue));
	}

	void CopyBufferToImage(VulkanContext* context, VmaBuffer* buffer, VmaImage* image, const VulkanQueue& queue)
	{
		VulkanCommand command = BeginSingleTimeCommand(context, queue);

		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { static_cast<uint32_t>(image->GetWidth()) , static_cast<uint32_t>(image->GetHeight()), static_cast<uint32_t>(image->GetDepth()) };

		vkCmdCopyBufferToImage(
			command.buffer,
			buffer->buffer,
			image->image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region);

		EndSingleTimeCommand(context, command, queue);
	}
}