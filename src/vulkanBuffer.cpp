#include <iostream>
#include "vulkanBuffer.h"
#include "vulkanCommands.h"
#include "macros.h"



// Constructor:
VulkanBuffer::VulkanBuffer(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, uint64_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryPropertyFlags, std::vector<uint32_t> queueFamilyIndices)
{
	this->logicalDevice = logicalDevice;
	this->physicalDevice = physicalDevice;
	this->size = size;

	// Create buffer:
	VkBufferCreateInfo createInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	createInfo.size = size;		// size in bytes
	createInfo.usage = usage;	// VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, etc.
	if (queueFamilyIndices.empty())
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;		// only one queue family will access this buffer
	else
	{
		createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;	// given list of queue families will access this buffer
		createInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size());	//
		createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
	}
	VKA(vkCreateBuffer(logicalDevice->device, &createInfo, nullptr, &buffer));

	// Get memory requirements:
	// due to alignment memoryRequirements.size might be different than given size.
	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(logicalDevice->device, buffer, &memoryRequirements);

	// Allocation info:
	VkMemoryAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
	allocateInfo.allocationSize = memoryRequirements.size;
	allocateInfo.memoryTypeIndex = FindMemoryType(memoryRequirements.memoryTypeBits, memoryPropertyFlags);

	// Allocate memory and bind buffer to it:	
	uint64_t offset = 0;
	VKA(vkAllocateMemory(logicalDevice->device, &allocateInfo, nullptr, &memory));
	VKA(vkBindBufferMemory(logicalDevice->device, buffer, memory, offset));
}



// Destructor:
VulkanBuffer::~VulkanBuffer()
{
	vkDestroyBuffer(logicalDevice->device, buffer, nullptr);
	vkFreeMemory(logicalDevice->device, memory, nullptr);
}



// Public:
void VulkanBuffer::CopyBuffer(VulkanLogicalDevice* logicalDevice, VulkanBuffer* srcBuffer, VulkanBuffer* dstBuffer, VkDeviceSize bufferSize)
{
	// Generator command buffer:
	VulkanCommands commands(1, logicalDevice, logicalDevice->transferQueue);
	VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	VKA(vkBeginCommandBuffer(commands.buffers[0], &beginInfo));

	// Queue copy command:
	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = bufferSize;
	vkCmdCopyBuffer(commands.buffers[0], srcBuffer->buffer, dstBuffer->buffer, 1, &copyRegion);
	vkEndCommandBuffer(commands.buffers[0]);

	// Submit command buffer:
	VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commands.buffers[0];
	VKA(vkQueueSubmit(logicalDevice->transferQueue.queue, 1, &submitInfo, VK_NULL_HANDLE));
	VKA(vkQueueWaitIdle(logicalDevice->transferQueue.queue));
}



// Private:
uint32_t VulkanBuffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
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