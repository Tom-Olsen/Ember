#include "vulkanVertexBuffer.h"
#include "macros.h"



// Constructor:
#ifdef RESIZEABLE_BAR // No staging buffer:
VulkanVertexBuffer::VulkanVertexBuffer(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, Mesh* mesh)
{
	this->logicalDevice = logicalDevice;
	this->physicalDevice = physicalDevice;

	// Create buffer:
	VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	VkDeviceSize bufferSize = mesh->SizeOfBuffer();
	this->buffer = std::make_unique<VulkanBuffer>(logicalDevice, physicalDevice, bufferSize, usage, memoryPropertyFlags);

	// Copy positions and colors:
	void* data;
	VKA(vkMapMemory(logicalDevice->device, buffer->memory, 0, bufferSize, 0, &data));
	memcpy(data, mesh->GetPositions().data(), mesh->SizeOfPositions());
	memcpy(static_cast<char*>(data) + mesh->SizeOfPositions(), mesh->GetColors().data(), mesh->SizeOfColors());
	vkUnmapMemory(logicalDevice->device, buffer->memory);
}
void VulkanVertexBuffer::UpdateBuffer(Mesh* mesh)
{
	VkDeviceSize bufferSize = mesh->SizeOfBuffer();
	vkQueueWaitIdle(logicalDevice->graphicsQueue.queue);	// wait for previous render calls to finish

	// Old buffer is big enough:
	if (bufferSize != buffer->size)
	{
		// Copy positions and colors:
		void* data;
		VKA(vkMapMemory(logicalDevice->device, buffer->memory, 0, bufferSize, 0, &data));
		memcpy(data, mesh->GetPositions().data(), mesh->SizeOfPositions());
		memcpy(static_cast<char*>(data) + mesh->SizeOfPositions(), mesh->GetColors().data(), mesh->SizeOfColors());
		vkUnmapMemory(logicalDevice->device, buffer->memory);
	}
	// Old buffer is too small:
	else
	{
		// Recreate buffer:
		VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		this->buffer = std::make_unique<VulkanBuffer>(logicalDevice, physicalDevice, bufferSize, usage, memoryPropertyFlags);

		// Copy positions and colors:
		void* data;
		VKA(vkMapMemory(logicalDevice->device, buffer->memory, 0, bufferSize, 0, &data));
		memcpy(data, mesh->GetPositions().data(), mesh->SizeOfPositions());
		memcpy(static_cast<char*>(data) + mesh->SizeOfPositions(), mesh->GetColors().data(), mesh->SizeOfColors());
		vkUnmapMemory(logicalDevice->device, buffer->memory);
	}
}
#else // With Staging buffer:
VulkanVertexBuffer::VulkanVertexBuffer(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, Mesh* mesh)
{
	this->logicalDevice = logicalDevice;
	this->physicalDevice = physicalDevice;

	// Create buffer:
	VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	VkDeviceSize bufferSize = mesh->GetSizeOfBuffer();
	std::vector<uint32_t> queueFamilyIndices = { logicalDevice->graphicsQueue.familyIndex, logicalDevice->transferQueue.familyIndex };
	this->buffer = std::make_unique<VulkanBuffer>(logicalDevice, physicalDevice, bufferSize, usage, memoryPropertyFlags, queueFamilyIndices);

	// Load data into staging buffer:
	VulkanBuffer stagingBuffer(logicalDevice, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	void* data;
	VKA(vkMapMemory(logicalDevice->device, stagingBuffer.memory, 0, bufferSize, 0, &data));
	memcpy(data, mesh->GetPositions().data(), mesh->GetSizeOfPositions());
	memcpy(static_cast<char*>(data) + mesh->GetSizeOfPositions(), mesh->GetColors().data(), mesh->GetSizeOfColors());
	vkUnmapMemory(logicalDevice->device, stagingBuffer.memory);

	// Copy data from staging to vertex buffer:
	VulkanBuffer::CopyBuffer(logicalDevice, &stagingBuffer, buffer.get(), bufferSize);
}
void VulkanVertexBuffer::UpdateBuffer(Mesh* mesh)
{
	VkDeviceSize bufferSize = mesh->GetSizeOfBuffer();
	vkQueueWaitIdle(logicalDevice->graphicsQueue.queue);	// wait for previous render calls to finish

	// Old buffer is big enough:
	if (bufferSize != buffer->size)
	{
		// Load data into staging buffer:
		VulkanBuffer stagingBuffer(logicalDevice, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		void* data;
		VKA(vkMapMemory(logicalDevice->device, stagingBuffer.memory, 0, bufferSize, 0, &data));
		memcpy(data, mesh->GetPositions().data(), mesh->GetSizeOfPositions());
		memcpy(static_cast<char*>(data) + mesh->GetSizeOfPositions(), mesh->GetColors().data(), mesh->GetSizeOfColors());
		vkUnmapMemory(logicalDevice->device, stagingBuffer.memory);

		// Copy data from staging to vertex buffer:
		VulkanBuffer::CopyBuffer(logicalDevice, &stagingBuffer, buffer.get(), bufferSize);
	}
	// Old buffer is too small:
	else
	{
		// Recreate buffer:
		VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		std::vector<uint32_t> queueFamilyIndices = { logicalDevice->graphicsQueue.familyIndex, logicalDevice->transferQueue.familyIndex };
		this->buffer = std::make_unique<VulkanBuffer>(logicalDevice, physicalDevice, bufferSize, usage, memoryPropertyFlags, queueFamilyIndices);

		// Load data into staging buffer:
		VulkanBuffer stagingBuffer(logicalDevice, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		void* data;
		VKA(vkMapMemory(logicalDevice->device, stagingBuffer.memory, 0, bufferSize, 0, &data));
		memcpy(data, mesh->GetPositions().data(), mesh->GetSizeOfPositions());
		memcpy(static_cast<char*>(data) + mesh->GetSizeOfPositions(), mesh->GetColors().data(), mesh->GetSizeOfColors());
		vkUnmapMemory(logicalDevice->device, stagingBuffer.memory);

		// Copy data from staging to vertex buffer:
		VulkanBuffer::CopyBuffer(logicalDevice, &stagingBuffer, buffer.get(), bufferSize);
	}
}
#endif



// Destructor:
VulkanVertexBuffer::~VulkanVertexBuffer()
{

}