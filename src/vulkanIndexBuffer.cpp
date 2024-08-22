#include "vulkanIndexBuffer.h"
#include "macros.h"



// Constructor:
//#define RESIZEABLE_BAR
#ifdef RESIZEABLE_BAR // No staging buffer:
VulkanIndexBuffer::VulkanIndexBuffer(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, Mesh* mesh)
{
	VkBufferUsageFlags usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	VkDeviceSize bufferSize = 3 * mesh->SizeOfTriangles();

	this->buffer = std::make_unique<VulkanBuffer>(logicalDevice, physicalDevice, bufferSize, usage, memoryPropertyFlags);

	void* data;
	VKA(vkMapMemory(logicalDevice->device, buffer->memory, 0, bufferSize, 0, &data));

	// Copy triangle indexes:
	memcpy(data, mesh->GetTrianglesUnrolled(), bufferSize);
	vkUnmapMemory(logicalDevice->device, buffer->memory);
}
#else // With Staging buffer:
VulkanIndexBuffer::VulkanIndexBuffer(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, Mesh* mesh)
{
	// Create buffer:
	VkBufferUsageFlags usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	VkDeviceSize bufferSize = 3 * mesh->SizeOfTriangles();
	this->buffer = std::make_unique<VulkanBuffer>(logicalDevice, physicalDevice, bufferSize, usage, memoryPropertyFlags);

	// Load data into staging buffer:
	VulkanBuffer stagingBuffer(logicalDevice, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	void* data;
	VKA(vkMapMemory(logicalDevice->device, stagingBuffer.memory, 0, bufferSize, 0, &data));
	memcpy(data, mesh->GetTrianglesUnrolled(), bufferSize);
	vkUnmapMemory(logicalDevice->device, stagingBuffer.memory);

	// Generator command buffer:
	VulkanCommands commands(1, logicalDevice);
	VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	VKA(vkBeginCommandBuffer(commands.buffers[0], &beginInfo));

	// Queue copy command:
	VkBufferCopy copyRegion = { 0, 0, bufferSize };
	vkCmdCopyBuffer(commands.buffers[0], stagingBuffer.buffer, buffer->buffer, 1, &copyRegion);
	vkEndCommandBuffer(commands.buffers[0]);

	// Submit command buffer:
	VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commands.buffers[0];
	VKA(vkQueueSubmit(logicalDevice->graphicsQueue.queue, 1, &submitInfo, VK_NULL_HANDLE));
	vkQueueWaitIdle(logicalDevice->graphicsQueue.queue);
}
#endif



// Destructor:
VulkanIndexBuffer::~VulkanIndexBuffer()
{

}