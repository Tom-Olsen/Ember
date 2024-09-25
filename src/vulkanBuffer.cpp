#include <iostream>
#include "vulkanBuffer.h"
#include "vulkanCommands.h"
#include "vulkanHelper.h"
#include "macros.h"



// Constructor:
VulkanBuffer::VulkanBuffer(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, uint64_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryPropertyFlags, const std::vector<uint32_t>& queueFamilyIndices)
{
	this->logicalDevice = logicalDevice;
	this->physicalDevice = physicalDevice;
	this->size = size;

	// Create buffer:
	VkBufferCreateInfo createInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	createInfo.size = size;		// size in bytes
	createInfo.usage = usage;	// VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, etc.
	if (queueFamilyIndices.size() <= 1)
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
	allocateInfo.memoryTypeIndex = VulkanHelper::FindMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, memoryPropertyFlags);

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
void VulkanBuffer::CopyBufferToBuffer(VulkanLogicalDevice* logicalDevice, VulkanBuffer* srcBuffer, VulkanBuffer* dstBuffer, VkDeviceSize bufferSize, const VulkanQueue& queue)
{
	VulkanCommands commands = VulkanHelper::BeginSingleTimeCommands(logicalDevice, queue);

	// Queue copy command:
	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = bufferSize;
	vkCmdCopyBuffer(commands.buffers[0], srcBuffer->buffer, dstBuffer->buffer, 1, &copyRegion);

	VulkanHelper::EndSingleTimeCommands(logicalDevice, commands, queue);
}



// Static:
VulkanBuffer VulkanBuffer::StagingBuffer(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, uint64_t bufferSize, void* inputData)
{
	// Create buffer:
	VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	VulkanBuffer stagingBuffer(logicalDevice, physicalDevice, bufferSize, usage, memoryPropertyFlags);

	// Load data into buffer:
	void* data;
	VKA(vkMapMemory(logicalDevice->device, stagingBuffer.memory, 0, bufferSize, 0, &data));
	memcpy(data, inputData, static_cast<size_t>(bufferSize));
	vkUnmapMemory(logicalDevice->device, stagingBuffer.memory);

	return stagingBuffer;
}
VulkanBuffer VulkanBuffer::StagingBuffer(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, const std::vector<uint64_t>& bufferSizes, const std::vector<void*>& inputDatas)
{
	// Check if bufferSizes and inputDatas have the same size:
	if (bufferSizes.size() != inputDatas.size())
		throw std::runtime_error("bufferSizes and inputDatas must have the same size!");

	// Calculate total buffer size = sum of all buffer sizes:
	uint64_t totalBufferSize = 0;
	for (uint64_t bufferSize : bufferSizes)
		totalBufferSize += bufferSize;

	// Create buffer:
	VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	VulkanBuffer stagingBuffer(logicalDevice, physicalDevice, totalBufferSize, usage, memoryPropertyFlags);

	// Load data into buffer:
	void* data;
	VKA(vkMapMemory(logicalDevice->device, stagingBuffer.memory, 0, totalBufferSize, 0, &data));

	uint64_t offset = 0;
	for (uint64_t i = 0; i < inputDatas.size(); i++)
	{
		memcpy(static_cast<char*>(data) + offset, inputDatas[i], static_cast<size_t>(bufferSizes[i]));
		offset += bufferSizes[i];
	}
	vkUnmapMemory(logicalDevice->device, stagingBuffer.memory);

	return stagingBuffer;
}