#include "vulkanUniformBuffer.h"
#include "vulkanMacros.h"
#include "gameobject.h"



// Constructor:
VulkanUniformBuffer::VulkanUniformBuffer(VulkanContext* context, UniformBufferBlock* uniformBufferBlock)
{
	this->context = context;
	this->uniformBufferBlock = uniformBufferBlock;

	// Create buffer:
	VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferInfo.size = uniformBufferBlock->size;
	bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
	allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
	allocInfo.requiredFlags = 0; 
	allocInfo.preferredFlags = 0;

	this->buffer = std::make_unique<VmaBuffer>(context, bufferInfo, allocInfo);

	// Get deviceData pointer:
	VmaAllocationInfo info;
	vmaGetAllocationInfo(context->Allocator(), buffer->allocation, &info);
	deviceData = info.pMappedData;

	// Allocate host data:
	hostData.resize(uniformBufferBlock->size);
}



// Destructor:
VulkanUniformBuffer::~VulkanUniformBuffer()
{
	VKA(vkDeviceWaitIdle(context->LogicalDevice()));
}



// Public:
void VulkanUniformBuffer::UpdateBuffer()
{
	memcpy(deviceData, hostData.data(), hostData.size());
}



// Getters.
uint32_t VulkanUniformBuffer::GetSize()
{
	return uniformBufferBlock->size;

}



// Setters:
template<typename T>
void VulkanUniformBuffer::SetValue(const std::string& memberName, const T& value)
{
	UniformBufferMember* member = uniformBufferBlock->GetMember(memberName);
	if (member != nullptr)
		memcpy(hostData.data() + member->offset, &value, member->size);
}
template<typename T>
void VulkanUniformBuffer::SetValue(const std::string& arrayName, uint32_t arrayIndex, const T& value)
{
	UniformBufferMember* member = uniformBufferBlock->GetMember(arrayName);
	if (member != nullptr)
	{
		UniformBufferMember* subMember = member->subMembers.at(arrayName + "[" + std::to_string(arrayIndex) + "]");
		if (subMember != nullptr)
			memcpy(hostData.data() + subMember->offset, &value, subMember->size);
	}
}



// Explicit template instantiation:
template void VulkanUniformBuffer::SetValue<int>(const std::string& memberName, const int& value);
template void VulkanUniformBuffer::SetValue<bool>(const std::string& memberName, const bool& value);
template void VulkanUniformBuffer::SetValue<float>(const std::string& memberName, const float& value);
template void VulkanUniformBuffer::SetValue<Float2>(const std::string& memberName, const Float2& value);
template void VulkanUniformBuffer::SetValue<Float3>(const std::string& memberName, const Float3& value);
template void VulkanUniformBuffer::SetValue<Float4>(const std::string& memberName, const Float4& value);
template void VulkanUniformBuffer::SetValue<Float4x4>(const std::string& memberName, const Float4x4& value);

template void VulkanUniformBuffer::SetValue<int>(const std::string& memberName, uint32_t arrayIndex, const int& value);
template void VulkanUniformBuffer::SetValue<bool>(const std::string& memberName, uint32_t arrayIndex, const bool& value);
template void VulkanUniformBuffer::SetValue<float>(const std::string& memberName, uint32_t arrayIndex, const float& value);
template void VulkanUniformBuffer::SetValue<Float2>(const std::string& memberName, uint32_t arrayIndex, const Float2& value);
template void VulkanUniformBuffer::SetValue<Float3>(const std::string& memberName, uint32_t arrayIndex, const Float3& value);
template void VulkanUniformBuffer::SetValue<Float4>(const std::string& memberName, uint32_t arrayIndex, const Float4& value);
template void VulkanUniformBuffer::SetValue<Float4x4>(const std::string& memberName, uint32_t arrayIndex, const Float4x4& value);