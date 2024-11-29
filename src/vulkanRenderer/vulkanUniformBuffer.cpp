#include "vulkanUniformBuffer.h"
#include "vulkanMacros.h"
#include "gameobject.h"
#include "logger.h"



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

	this->buffer = std::make_shared<VmaBuffer>(context, bufferInfo, allocInfo);

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
bool VulkanUniformBuffer::SetValue(const std::string& memberName, const T& value)
{
	// redirect bool to int:
	if constexpr (std::is_same<T, bool>::value)
	{
		int intValue = static_cast<int>(value);
		return SetValue<int>(memberName, intValue);
	}

	UniformBufferMember* member = uniformBufferBlock->GetMember(memberName);
	if (member != nullptr)
		return CheckAndUpdateData(value, member->offset, member->size);

	// value not found:
	return false;
}
template<typename T>
bool VulkanUniformBuffer::SetValue(const std::string& arrayName, uint32_t arrayIndex, const T& value)
{
	// redirect bool to int:
	if constexpr (std::is_same<T, bool>::value)
	{
		int intValue = static_cast<int>(value);
		return SetValue<int>(arrayName, arrayIndex, intValue);
	}

	UniformBufferMember* member = uniformBufferBlock->GetMember(arrayName);
	if (member != nullptr)
	{
		UniformBufferMember* subMember = member->GetSubMember(arrayName + "[" + std::to_string(arrayIndex) + "]");
		if (subMember != nullptr)
			return CheckAndUpdateData(value, subMember->offset, subMember->size);
	}

	// value not found:
	return false;
}
template<typename T>
bool VulkanUniformBuffer::SetValue(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const T& value)
{
	// redirect bool to int:
	if constexpr (std::is_same<T, bool>::value)
	{
		int intValue = static_cast<int>(value);
		return SetValue<int>(arrayName, arrayIndex, memberName, intValue);
	}

	UniformBufferMember* member = uniformBufferBlock->GetMember(arrayName);
	if (member != nullptr)
	{
		UniformBufferMember* subMember = member->GetSubMember(arrayName + "[" + std::to_string(arrayIndex) + "]");
		if (subMember != nullptr)
		{
			UniformBufferMember* subSubMember = subMember->GetSubMember(memberName);
			if (subSubMember != nullptr)
				return CheckAndUpdateData(value, subSubMember->offset, subSubMember->size);
		}
	}

	// value not found:
	return false;
}
template<typename T>
bool VulkanUniformBuffer::SetValue(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const T& value)
{
	// redirect bool to int:
	if constexpr (std::is_same<T, bool>::value)
	{
		int intValue = static_cast<int>(value);
		return SetValue<int>(arrayName, arrayIndex, subArrayName, subArrayIndex, intValue);
	}

	UniformBufferMember* member = uniformBufferBlock->GetMember(arrayName);
	if (member != nullptr)
	{
		UniformBufferMember* subMember = member->GetSubMember(arrayName + "[" + std::to_string(arrayIndex) + "]");
		if (subMember != nullptr)
		{
			UniformBufferMember* subSubMember = subMember->GetSubMember(subArrayName);
			if (subSubMember != nullptr)
			{
				UniformBufferMember* subSubSubMember = subSubMember->GetSubMember(subArrayName + "[" + std::to_string(subArrayIndex) + "]");
				if (subSubSubMember != nullptr)
					return CheckAndUpdateData(value, subSubSubMember->offset, subSubSubMember->size);
			}
		}
	}

	// value not found:
	return false;
}



// Private methods:
template<typename T>
bool VulkanUniformBuffer::CheckAndUpdateData(const T& value, uint32_t offset, uint32_t size)
{
	const void* oldData = hostData.data() + offset;
	const void* newData = &value;
	size_t dataSize = std::min(static_cast<size_t>(size), (sizeof(value)));

	// Compare old and new Data. If they are the same (0), return false:
	if (std::memcmp(oldData, newData, dataSize) == 0)
		return false;

	// Update hostData:
	std::memcpy(hostData.data() + offset, newData, dataSize);
	return true;
}



// Explicit template instantiation:
template bool VulkanUniformBuffer::CheckAndUpdateData<int>(const int& value, uint32_t offset, uint32_t size);
template bool VulkanUniformBuffer::CheckAndUpdateData<bool>(const bool& value, uint32_t offset, uint32_t size);
template bool VulkanUniformBuffer::CheckAndUpdateData<float>(const float& value, uint32_t offset, uint32_t size);
template bool VulkanUniformBuffer::CheckAndUpdateData<Float2>(const Float2& value, uint32_t offset, uint32_t size);
template bool VulkanUniformBuffer::CheckAndUpdateData<Float3>(const Float3& value, uint32_t offset, uint32_t size);
template bool VulkanUniformBuffer::CheckAndUpdateData<Float4>(const Float4& value, uint32_t offset, uint32_t size);
template bool VulkanUniformBuffer::CheckAndUpdateData<Float4x4>(const Float4x4& value, uint32_t offset, uint32_t size);

template bool VulkanUniformBuffer::SetValue<int>(const std::string& memberName, const int& value);
template bool VulkanUniformBuffer::SetValue<bool>(const std::string& memberName, const bool& value);
template bool VulkanUniformBuffer::SetValue<float>(const std::string& memberName, const float& value);
template bool VulkanUniformBuffer::SetValue<Float2>(const std::string& memberName, const Float2& value);
template bool VulkanUniformBuffer::SetValue<Float3>(const std::string& memberName, const Float3& value);
template bool VulkanUniformBuffer::SetValue<Float4>(const std::string& memberName, const Float4& value);
template bool VulkanUniformBuffer::SetValue<Float4x4>(const std::string& memberName, const Float4x4& value);

template bool VulkanUniformBuffer::SetValue<int>(const std::string& arrayName, uint32_t arrayIndex, const int& value);
template bool VulkanUniformBuffer::SetValue<bool>(const std::string& arrayName, uint32_t arrayIndex, const bool& value);
template bool VulkanUniformBuffer::SetValue<float>(const std::string& arrayName, uint32_t arrayIndex, const float& value);
template bool VulkanUniformBuffer::SetValue<Float2>(const std::string& arrayName, uint32_t arrayIndex, const Float2& value);
template bool VulkanUniformBuffer::SetValue<Float3>(const std::string& arrayName, uint32_t arrayIndex, const Float3& value);
template bool VulkanUniformBuffer::SetValue<Float4>(const std::string& arrayName, uint32_t arrayIndex, const Float4& value);
template bool VulkanUniformBuffer::SetValue<Float4x4>(const std::string& arrayName, uint32_t arrayIndex, const Float4x4& value);

template bool VulkanUniformBuffer::SetValue<int>(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const int& value);
template bool VulkanUniformBuffer::SetValue<bool>(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const bool& value);
template bool VulkanUniformBuffer::SetValue<float>(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const float& value);
template bool VulkanUniformBuffer::SetValue<Float2>(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float2& value);
template bool VulkanUniformBuffer::SetValue<Float3>(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float3& value);
template bool VulkanUniformBuffer::SetValue<Float4>(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4& value);
template bool VulkanUniformBuffer::SetValue<Float4x4>(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4x4& value);

template bool VulkanUniformBuffer::SetValue<int>(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const int& value);
template bool VulkanUniformBuffer::SetValue<bool>(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const bool& value);
template bool VulkanUniformBuffer::SetValue<float>(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const float& value);
template bool VulkanUniformBuffer::SetValue<Float2>(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float2& value);
template bool VulkanUniformBuffer::SetValue<Float3>(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float3& value);
template bool VulkanUniformBuffer::SetValue<Float4>(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4& value);
template bool VulkanUniformBuffer::SetValue<Float4x4>(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4x4& value);