#include "uniformBuffer.h"
#include "vulkanContext.h"
#include "vmaBuffer.h"
#include "spirvReflect.h"



// Constructor/Destructor:
UniformBuffer::UniformBuffer(VulkanContext* pContext, UniformBufferBlock* pUniformBufferBlock)
{
	m_pContext = pContext;
	m_pUniformBufferBlock = pUniformBufferBlock;

	// Create buffer:
	VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferInfo.size = m_pUniformBufferBlock->size;
	bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
	allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
	allocInfo.requiredFlags = 0; 
	allocInfo.preferredFlags = 0;

	m_buffer = std::make_shared<VmaBuffer>(m_pContext, bufferInfo, allocInfo);

	// Get deviceData pointer:
	VmaAllocationInfo info;
	vmaGetAllocationInfo(m_pContext->GetVmaAllocator(), m_buffer->GetVmaAllocation(), &info);
	m_pDeviceData = info.pMappedData;

	// Allocate host data:
	m_hostData.resize(m_pUniformBufferBlock->size);
}
UniformBuffer::~UniformBuffer()
{
	m_pContext->WaitDeviceIdle();
}



// Public methods:
void UniformBuffer::UpdateBuffer()
{
	memcpy(m_pDeviceData, m_hostData.data(), m_hostData.size());
}



// Getters:
uint32_t UniformBuffer::GetSize() const
{
	return m_pUniformBufferBlock->size;
}
const std::shared_ptr<VmaBuffer>& UniformBuffer::GetVmaBuffer() const
{
	return m_buffer;
}



// Setters:
template<typename T>
bool UniformBuffer::SetValue(const std::string& memberName, const T& value)
{
	// redirect bool to int:
	if constexpr (std::is_same<T, bool>::value)
	{
		int intValue = static_cast<int>(value);
		return SetValue<int>(memberName, intValue);
	}

	UniformBufferMember* pMember = m_pUniformBufferBlock->GetMember(memberName);
	if (pMember != nullptr)
		return CheckAndUpdateData(value, pMember->offset, pMember->size);

	// value not found:
	return false;
}
template<typename T>
bool UniformBuffer::SetValue(const std::string& arrayName, uint32_t arrayIndex, const T& value)
{
	// redirect bool to int:
	if constexpr (std::is_same<T, bool>::value)
	{
		int intValue = static_cast<int>(value);
		return SetValue<int>(arrayName, arrayIndex, intValue);
	}

	UniformBufferMember* pMember = m_pUniformBufferBlock->GetMember(arrayName);
	if (pMember != nullptr)
	{
		UniformBufferMember* pSubMember = pMember->GetSubMember(arrayName + "[" + std::to_string(arrayIndex) + "]");
		if (pSubMember != nullptr)
			return CheckAndUpdateData(value, pSubMember->offset, pSubMember->size);
	}

	// value not found:
	return false;
}
template<typename T>
bool UniformBuffer::SetValue(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const T& value)
{
	// redirect bool to int:
	if constexpr (std::is_same<T, bool>::value)
	{
		int intValue = static_cast<int>(value);
		return SetValue<int>(arrayName, arrayIndex, memberName, intValue);
	}

	UniformBufferMember* pMember = m_pUniformBufferBlock->GetMember(arrayName);
	if (pMember != nullptr)
	{
		UniformBufferMember* pSubMember = pMember->GetSubMember(arrayName + "[" + std::to_string(arrayIndex) + "]");
		if (pSubMember != nullptr)
		{
			UniformBufferMember* pSubSubMember = pSubMember->GetSubMember(memberName);
			if (pSubSubMember != nullptr)
				return CheckAndUpdateData(value, pSubSubMember->offset, pSubSubMember->size);
		}
	}

	// value not found:
	return false;
}
template<typename T>
bool UniformBuffer::SetValue(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const T& value)
{
	// redirect bool to int:
	if constexpr (std::is_same<T, bool>::value)
	{
		int intValue = static_cast<int>(value);
		return SetValue<int>(arrayName, arrayIndex, subArrayName, subArrayIndex, intValue);
	}

	UniformBufferMember* pMember = m_pUniformBufferBlock->GetMember(arrayName);
	if (pMember != nullptr)
	{
		UniformBufferMember* pSubMember = pMember->GetSubMember(arrayName + "[" + std::to_string(arrayIndex) + "]");
		if (pSubMember != nullptr)
		{
			UniformBufferMember* pSubSubMember = pSubMember->GetSubMember(subArrayName);
			if (pSubSubMember != nullptr)
			{
				UniformBufferMember* pSubSubSubMember = pSubSubMember->GetSubMember(subArrayName + "[" + std::to_string(subArrayIndex) + "]");
				if (pSubSubSubMember != nullptr)
					return CheckAndUpdateData(value, pSubSubSubMember->offset, pSubSubSubMember->size);
			}
		}
	}

	// value not found:
	return false;
}



// Private methods:
template<typename T>
bool UniformBuffer::CheckAndUpdateData(const T& value, uint32_t offset, uint32_t size)
{
	const void* oldData = m_hostData.data() + offset;
	const void* newData = &value;
	size_t dataSize = std::min(static_cast<size_t>(size), (sizeof(value)));

	// Compare old and new Data. If they are the same (0), return false:
	if (std::memcmp(oldData, newData, dataSize) == 0)
		return false;

	// Update hostData:
	std::memcpy(m_hostData.data() + offset, newData, dataSize);
	return true;
}



// Explicit template instantiation:
template bool UniformBuffer::CheckAndUpdateData<int>(const int& value, uint32_t offset, uint32_t size);
template bool UniformBuffer::CheckAndUpdateData<bool>(const bool& value, uint32_t offset, uint32_t size);
template bool UniformBuffer::CheckAndUpdateData<float>(const float& value, uint32_t offset, uint32_t size);
template bool UniformBuffer::CheckAndUpdateData<Float2>(const Float2& value, uint32_t offset, uint32_t size);
template bool UniformBuffer::CheckAndUpdateData<Float3>(const Float3& value, uint32_t offset, uint32_t size);
template bool UniformBuffer::CheckAndUpdateData<Float4>(const Float4& value, uint32_t offset, uint32_t size);
template bool UniformBuffer::CheckAndUpdateData<Float4x4>(const Float4x4& value, uint32_t offset, uint32_t size);

template bool UniformBuffer::SetValue<int>(const std::string& memberName, const int& value);
template bool UniformBuffer::SetValue<bool>(const std::string& memberName, const bool& value);
template bool UniformBuffer::SetValue<float>(const std::string& memberName, const float& value);
template bool UniformBuffer::SetValue<Float2>(const std::string& memberName, const Float2& value);
template bool UniformBuffer::SetValue<Float3>(const std::string& memberName, const Float3& value);
template bool UniformBuffer::SetValue<Float4>(const std::string& memberName, const Float4& value);
template bool UniformBuffer::SetValue<Float4x4>(const std::string& memberName, const Float4x4& value);

template bool UniformBuffer::SetValue<int>(const std::string& arrayName, uint32_t arrayIndex, const int& value);
template bool UniformBuffer::SetValue<bool>(const std::string& arrayName, uint32_t arrayIndex, const bool& value);
template bool UniformBuffer::SetValue<float>(const std::string& arrayName, uint32_t arrayIndex, const float& value);
template bool UniformBuffer::SetValue<Float2>(const std::string& arrayName, uint32_t arrayIndex, const Float2& value);
template bool UniformBuffer::SetValue<Float3>(const std::string& arrayName, uint32_t arrayIndex, const Float3& value);
template bool UniformBuffer::SetValue<Float4>(const std::string& arrayName, uint32_t arrayIndex, const Float4& value);
template bool UniformBuffer::SetValue<Float4x4>(const std::string& arrayName, uint32_t arrayIndex, const Float4x4& value);

template bool UniformBuffer::SetValue<int>(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const int& value);
template bool UniformBuffer::SetValue<bool>(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const bool& value);
template bool UniformBuffer::SetValue<float>(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const float& value);
template bool UniformBuffer::SetValue<Float2>(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float2& value);
template bool UniformBuffer::SetValue<Float3>(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float3& value);
template bool UniformBuffer::SetValue<Float4>(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4& value);
template bool UniformBuffer::SetValue<Float4x4>(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4x4& value);

template bool UniformBuffer::SetValue<int>(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const int& value);
template bool UniformBuffer::SetValue<bool>(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const bool& value);
template bool UniformBuffer::SetValue<float>(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const float& value);
template bool UniformBuffer::SetValue<Float2>(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float2& value);
template bool UniformBuffer::SetValue<Float3>(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float3& value);
template bool UniformBuffer::SetValue<Float4>(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4& value);
template bool UniformBuffer::SetValue<Float4x4>(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4x4& value);