#include "vulkanUniformBuffer.h"
#include "macros.h"



// Constructor:
VulkanUniformBuffer::VulkanUniformBuffer(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, uint64_t bufferSize)
{
	this->bufferSize = bufferSize;
	this->logicalDevice = logicalDevice;
	this->physicalDevice = physicalDevice;

	// Create buffer:
	VkBufferUsageFlags usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	std::vector<uint32_t> queueFamilyIndices = {};
	this->buffer = std::make_unique<VulkanBuffer>(logicalDevice, physicalDevice, bufferSize, usage, memoryPropertyFlags, queueFamilyIndices);
	
	// Permanently map data void pointer to buffer:
	VKA(vkMapMemory(logicalDevice->device, buffer->memory, 0, bufferSize, 0, &data));
}



// Destructor:
VulkanUniformBuffer::~VulkanUniformBuffer()
{

}



// Public:
template<typename T>
void VulkanUniformBuffer::UpdateBuffer(const T& datastruct)
{
	memcpy(data, &datastruct, sizeof(datastruct));
}



// Explicit instantiations:
template void VulkanUniformBuffer::UpdateBuffer<GlobalUniformObject>(const GlobalUniformObject& datastruct);