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