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
	//buffer = VulkanBuffer(logicalDevice, physicalDevice, bufferSize, usage, memoryPropertyFlags, queueFamilyIndices);
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
template void VulkanUniformBuffer::UpdateBuffer<UniformObject>(const UniformObject& datastruct);
template void VulkanUniformBuffer::UpdateBuffer<float>(const float& datastruct);
//template void VulkanUniformBuffer::UpdateBuffer<Float2>(const Float2& datastruct);
//template void VulkanUniformBuffer::UpdateBuffer<Float3>(const Float3& datastruct);
//template void VulkanUniformBuffer::UpdateBuffer<Float4>(const Float4& datastruct);
//template void VulkanUniformBuffer::UpdateBuffer<Float2x2>(const Float2x2& datastruct);
//template void VulkanUniformBuffer::UpdateBuffer<Float2x3>(const Float2x3& datastruct);
//template void VulkanUniformBuffer::UpdateBuffer<Float2x4>(const Float2x4& datastruct);
//template void VulkanUniformBuffer::UpdateBuffer<Float3x2>(const Float3x2& datastruct);
//template void VulkanUniformBuffer::UpdateBuffer<Float3x3>(const Float3x3& datastruct);
//template void VulkanUniformBuffer::UpdateBuffer<Float3x4>(const Float3x4& datastruct);
//template void VulkanUniformBuffer::UpdateBuffer<Float4x2>(const Float4x2& datastruct);
//template void VulkanUniformBuffer::UpdateBuffer<Float4x3>(const Float4x3& datastruct);
//template void VulkanUniformBuffer::UpdateBuffer<Float4x4>(const Float4x4& datastruct);