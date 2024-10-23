#include "vulkanUniformBuffer.h"
#include "macros.h"



// Constructor:
VulkanUniformBuffer::VulkanUniformBuffer(VulkanContext* context, uint64_t size)
{
	this->size = size;
	this->context = context;

	// Create buffer:
	VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferInfo.size = size;
	bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
	allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
	allocInfo.requiredFlags = 0; 
	allocInfo.preferredFlags = 0;

	this->buffer = std::make_unique<VmaBuffer>(context, bufferInfo, allocInfo);

	// get data pointer:
	VmaAllocationInfo info;
	vmaGetAllocationInfo(context->Allocator(), buffer->allocation, &info);
	data = info.pMappedData;
}



// Destructor:
VulkanUniformBuffer::~VulkanUniformBuffer()
{
	VKA(vkDeviceWaitIdle(context->LogicalDevice()));
}



// Public:
template<typename T>
void VulkanUniformBuffer::UpdateBuffer(const T& datastruct)
{
	memcpy(data, &datastruct, sizeof(datastruct));
}



// Explicit instantiations:
template void VulkanUniformBuffer::UpdateBuffer<RenderMatrizes>(const RenderMatrizes& datastruct);