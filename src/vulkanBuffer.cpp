#include "vulkanBuffer.h"
#include <iostream>
#include "macros.h"



// Constructor:
VulkanBuffer::VulkanBuffer(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, uint64_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryPropertyFlags)
{
	this->logicalDevice = logicalDevice;
	this->physicalDevice = physicalDevice;

	// Create buffer:
	VkBufferCreateInfo createInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	createInfo.size = size;
	createInfo.usage = usage;
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



// private:
uint32_t VulkanBuffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice->device, &deviceMemoryProperties);

	for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++)
	{
		// Check if required memory type is allowed
		if ((typeFilter & (1 << i)) != 0)
			// Check if at least the required properties are satisfied
			if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;
	}

	throw std::runtime_error("Failed to find suitable memory type!");
	return -1;
}