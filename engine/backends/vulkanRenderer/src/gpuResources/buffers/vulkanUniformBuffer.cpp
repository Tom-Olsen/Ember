#include "vulkanUniformBuffer.h"
#include "vk_mem_alloc.h"
#include "vmaBuffer.h"
#include "vulkanContext.h"
#include "vulkanPhysicalDevice.h"
#include <cassert>
#include <cstring>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	UniformBuffer::UniformBuffer(emberBufferLayout::BufferLayout bufferLayout)
		: m_bufferLayout(bufferLayout)
	{
		// Compute buffer size:
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(Context::GetPhysicalDevice()->GetVkPhysicalDevice(), &deviceProperties);
		uint32_t alignment = deviceProperties.limits.minUniformBufferOffsetAlignment;
		uint32_t structSize = bufferLayout.GetSize();
		m_alignedSize = ((structSize + alignment - 1) / alignment) * alignment;
		uint32_t bufferSize = Context::GetFramesInFlight() * m_alignedSize;
		
		// Create buffer:
		BufferCreateInfo bufferInfo = {};
		bufferInfo.size = bufferSize;
		bufferInfo.usages = BufferUsageFlags::uniform_buffer_bit;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		AllocationCreateInfo allocInfo = {};
		allocInfo.usages = MemoryUsages::auto_prefer_host;
		allocInfo.flags = AllocationCreateFlags::mapped_bit | AllocationCreateFlags::host_access_sequential_write_bit;
		allocInfo.requiredFlags = 0;
		allocInfo.preferredFlags = 0;

		m_pBuffer = std::make_unique<VmaBuffer>(bufferInfo, allocInfo);

		// Get mapped deviceData pointer:
		VmaAllocationInfo info;
		vmaGetAllocationInfo(Context::GetVmaAllocator(), m_pBuffer->GetVmaAllocation(), &info);
		m_pDeviceData = static_cast<uint8_t*>(info.pMappedData);

		// Allocate host data:
		m_hostData.resize(m_bufferLayout.GetSize());
	}
	UniformBuffer::~UniformBuffer()
	{

	}



	// Update buffer:
	void UniformBuffer::UpdateBuffer(uint32_t frameIndex)
	{
		assert(frameIndex < Context::GetFramesInFlight());
		uint32_t offset = frameIndex * m_alignedSize;
		memcpy(m_pDeviceData + offset, m_hostData.data(), m_hostData.size());
	}



	// Getters:
	uint32_t UniformBuffer::GetAlignedSubBufferSize()
	{
		return m_alignedSize;
	}
	uint32_t UniformBuffer::GetBufferOffset(uint32_t frameIndex)
	{
		assert(frameIndex < Context::GetFramesInFlight());
		return frameIndex * m_alignedSize;
	}
	// Simple members:
	int UniformBuffer::GetInt(const std::string& memberName) const
	{
		return GetValue<int>(memberName);
	}
	bool UniformBuffer::GetBool(const std::string& memberName) const
	{
		return GetValue<bool>(memberName);
	}
	float UniformBuffer::GetFloat(const std::string& memberName) const
	{
		return GetValue<float>(memberName);
	}
	Float2 UniformBuffer::GetFloat2(const std::string& memberName) const
	{
		return GetValue<Float2>(memberName);
	}
	Float3 UniformBuffer::GetFloat3(const std::string& memberName) const
	{
		return GetValue<Float3>(memberName);
	}
	Float4 UniformBuffer::GetFloat4(const std::string& memberName) const
	{
		return GetValue<Float4>(memberName);
	}
	Float4x4 UniformBuffer::GetFloat4x4(const std::string& memberName) const
	{
		return GetValue<Float4x4>(memberName);
	}
	// Array members:
	int UniformBuffer::GetInt(const std::string& arrayName, uint32_t arrayIndex) const
	{
		return GetValue<int>(arrayName, arrayIndex);
	}
	bool UniformBuffer::GetBool(const std::string& arrayName, uint32_t arrayIndex) const
	{
		return GetValue<bool>(arrayName, arrayIndex);
	}
	float UniformBuffer::GetFloat(const std::string& arrayName, uint32_t arrayIndex) const
	{
		return GetValue<float>(arrayName, arrayIndex);
	}
	Float2 UniformBuffer::GetFloat2(const std::string& arrayName, uint32_t arrayIndex) const
	{
		return GetValue<Float2>(arrayName, arrayIndex);
	}
	Float3 UniformBuffer::GetFloat3(const std::string& arrayName, uint32_t arrayIndex) const
	{
		return GetValue<Float3>(arrayName, arrayIndex);
	}
	Float4 UniformBuffer::GetFloat4(const std::string& arrayName, uint32_t arrayIndex) const
	{
		return GetValue<Float4>(arrayName, arrayIndex);
	}
	Float4x4 UniformBuffer::GetFloat4x4(const std::string& arrayName, uint32_t arrayIndex) const
	{
		return GetValue<Float4x4>(arrayName, arrayIndex);
	}
	// Array of structs members:
	int UniformBuffer::GetInt(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const
	{
		return GetValue<int>(arrayName, arrayIndex, memberName);
	}
	bool UniformBuffer::GetBool(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const
	{
		return GetValue<bool>(arrayName, arrayIndex, memberName);
	}
	float UniformBuffer::GetFloat(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const
	{
		return GetValue<float>(arrayName, arrayIndex, memberName);
	}
	Float2 UniformBuffer::GetFloat2(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const
	{
		return GetValue<Float2>(arrayName, arrayIndex, memberName);
	}
	Float3 UniformBuffer::GetFloat3(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const
	{
		return GetValue<Float3>(arrayName, arrayIndex, memberName);
	}
	Float4 UniformBuffer::GetFloat4(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const
	{
		return GetValue<Float4>(arrayName, arrayIndex, memberName);
	}
	Float4x4 UniformBuffer::GetFloat4x4(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const
	{
		return GetValue<Float4x4>(arrayName, arrayIndex, memberName);
	}
	// Array of arrays members:
	int UniformBuffer::GetInt(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const
	{
		return GetValue<int>(arrayName, arrayIndex, subArrayName, subArrayIndex);
	}
	bool UniformBuffer::GetBool(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const
	{
		return GetValue<bool>(arrayName, arrayIndex, subArrayName, subArrayIndex);
	}
	float UniformBuffer::GetFloat(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const
	{
		return GetValue<float>(arrayName, arrayIndex, subArrayName, subArrayIndex);
	}
	Float2 UniformBuffer::GetFloat2(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const
	{
		return GetValue<Float2>(arrayName, arrayIndex, subArrayName, subArrayIndex);
	}
	Float3 UniformBuffer::GetFloat3(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const
	{
		return GetValue<Float3>(arrayName, arrayIndex, subArrayName, subArrayIndex);
	}
	Float4 UniformBuffer::GetFloat4(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const
	{
		return GetValue<Float4>(arrayName, arrayIndex, subArrayName, subArrayIndex);
	}
	Float4x4 UniformBuffer::GetFloat4x4(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const
	{
		return GetValue<Float4x4>(arrayName, arrayIndex, subArrayName, subArrayIndex);
	}



	// Setters:
	// Simple members:
	bool UniformBuffer::SetInt(const std::string& memberName, int value)
	{
		return SetValue(memberName, value);
	}
	bool UniformBuffer::SetBool(const std::string& memberName, bool value)
	{
		return SetValue(memberName, value);
	}
	bool UniformBuffer::SetFloat(const std::string& memberName, float value)
	{
		return SetValue(memberName, value);
	}
	bool UniformBuffer::SetFloat2(const std::string& memberName, const Float2& value)
	{
		return SetValue(memberName, value);
	}
	bool UniformBuffer::SetFloat3(const std::string& memberName, const Float3& value)
	{
		return SetValue(memberName, value);
	}
	bool UniformBuffer::SetFloat4(const std::string& memberName, const Float4& value)
	{
		return SetValue(memberName, value);
	}
	bool UniformBuffer::SetFloat4x4(const std::string& memberName, const Float4x4& value)
	{
		return SetValue(memberName, value);
	}
	// Array members:
	bool UniformBuffer::SetInt(const std::string& arrayName, uint32_t arrayIndex, int value)
	{
		return SetValue(arrayName, arrayIndex, value);
	}
	bool UniformBuffer::SetBool(const std::string& arrayName, uint32_t arrayIndex, bool value)
	{
		return SetValue(arrayName, arrayIndex, value);
	}
	bool UniformBuffer::SetFloat(const std::string& arrayName, uint32_t arrayIndex, float value)
	{
		return SetValue(arrayName, arrayIndex, value);
	}
	bool UniformBuffer::SetFloat2(const std::string& arrayName, uint32_t arrayIndex, const Float2& value)
	{
		return SetValue(arrayName, arrayIndex, value);
	}
	bool UniformBuffer::SetFloat3(const std::string& arrayName, uint32_t arrayIndex, const Float3& value)
	{
		return SetValue(arrayName, arrayIndex, value);
	}
	bool UniformBuffer::SetFloat4(const std::string& arrayName, uint32_t arrayIndex, const Float4& value)
	{
		return SetValue(arrayName, arrayIndex, value);
	}
	bool UniformBuffer::SetFloat4x4(const std::string& arrayName, uint32_t arrayIndex, const Float4x4& value)
	{
		return SetValue(arrayName, arrayIndex, value);
	}
	// Array of structs members:
	bool UniformBuffer::SetInt(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, int value)
	{
		return SetValue(arrayName, arrayIndex, memberName, value);
	}
	bool UniformBuffer::SetBool(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, bool value)
	{
		return SetValue(arrayName, arrayIndex, memberName, value);
	}
	bool UniformBuffer::SetFloat(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, float value)
	{
		return SetValue(arrayName, arrayIndex, memberName, value);
	}
	bool UniformBuffer::SetFloat2(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float2& value)
	{
		return SetValue(arrayName, arrayIndex, memberName, value);
	}
	bool UniformBuffer::SetFloat3(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float3& value)
	{
		return SetValue(arrayName, arrayIndex, memberName, value);
	}
	bool UniformBuffer::SetFloat4(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4& value)
	{
		return SetValue(arrayName, arrayIndex, memberName, value);
	}
	bool UniformBuffer::SetFloat4x4(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4x4& value)
	{
		return SetValue(arrayName, arrayIndex, memberName, value);
	}
	// Array of arrays members:
	bool UniformBuffer::SetInt(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, int value)
	{
		return SetValue(arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	bool UniformBuffer::SetBool(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, bool value)
	{
		return SetValue(arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	bool UniformBuffer::SetFloat(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, float value)
	{
		return SetValue(arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	bool UniformBuffer::SetFloat2(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float2& value)
	{
		return SetValue(arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	bool UniformBuffer::SetFloat3(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float3& value)
	{
		return SetValue(arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	bool UniformBuffer::SetFloat4(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4& value)
	{
		return SetValue(arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	bool UniformBuffer::SetFloat4x4(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4x4& value)
	{
		return SetValue(arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}



	// Private methods:
	// Getter templates, used in actual getters:
	template<typename T>
	T UniformBuffer::GetValue(const std::string& memberName) const
	{
		const emberBufferLayout::BufferMember* pMember = m_bufferLayout.GetMember(memberName);
		if (pMember != nullptr)
			return GetData<T>(pMember->GetOffset(), pMember->GetSize());

		// Could not find requested entry:
		return T();
	}
	template<typename T>
	T UniformBuffer::GetValue(const std::string& arrayName, uint32_t arrayIndex) const
	{
		const emberBufferLayout::BufferMember* pMember = m_bufferLayout.GetMember(arrayName);
		if (pMember != nullptr)
		{
			const emberBufferLayout::BufferMember* pSubMember = pMember->GetSubMember(arrayName + "[" + std::to_string(arrayIndex) + "]");
			if (pSubMember != nullptr)
				return GetData<T>(pSubMember->GetOffset(), pSubMember->GetSize());
		}

		// Could not find requested entry:
		return T();
	}
	template<typename T>
	T UniformBuffer::GetValue(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const
	{
		const emberBufferLayout::BufferMember* pMember = m_bufferLayout.GetMember(arrayName);
		if (pMember != nullptr)
		{
			const emberBufferLayout::BufferMember* pSubMember = pMember->GetSubMember(arrayName + "[" + std::to_string(arrayIndex) + "]");
			if (pSubMember != nullptr)
			{
				const emberBufferLayout::BufferMember* pSubSubMember = pSubMember->GetSubMember(memberName);
				if (pSubSubMember != nullptr)
					return GetData<T>(pSubSubMember->GetOffset(), pSubSubMember->GetSize());
			}
		}

		// Could not find requested entry:
		return T();
	}
	template<typename T>
	T UniformBuffer::GetValue(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const
	{
		const emberBufferLayout::BufferMember* pMember = m_bufferLayout.GetMember(arrayName);
		if (pMember != nullptr)
		{
			const emberBufferLayout::BufferMember* pSubMember = pMember->GetSubMember(arrayName + "[" + std::to_string(arrayIndex) + "]");
			if (pSubMember != nullptr)
			{
				const emberBufferLayout::BufferMember* pSubSubMember = pSubMember->GetSubMember(subArrayName);
				if (pSubSubMember != nullptr)
				{
					const emberBufferLayout::BufferMember* pSubSubSubMember = pSubSubMember->GetSubMember(subArrayName + "[" + std::to_string(subArrayIndex) + "]");
					if (pSubSubSubMember != nullptr)
						return GetData<T>(pSubSubSubMember->GetOffset(), pSubSubSubMember->GetSize());
				}
			}
		}

		// Could not find requested entry:
		return T();
	}



	// Setter templates, used in actual setters:
	template<typename T>
	bool UniformBuffer::SetValue(const std::string& memberName, const T& value)
	{
		const emberBufferLayout::BufferMember* pMember = m_bufferLayout.GetMember(memberName);
		if (pMember != nullptr)
			return CheckAndUpdateData(value, pMember->GetOffset(), pMember->GetSize());

		// Value not found:
		return false;
	}
	template<typename T>
	bool UniformBuffer::SetValue(const std::string& arrayName, uint32_t arrayIndex, const T& value)
	{
		const emberBufferLayout::BufferMember* pMember = m_bufferLayout.GetMember(arrayName);
		if (pMember != nullptr)
		{
			const emberBufferLayout::BufferMember* pSubMember = pMember->GetSubMember(arrayName + "[" + std::to_string(arrayIndex) + "]");
			if (pSubMember != nullptr)
				return CheckAndUpdateData(value, pSubMember->GetOffset(), pSubMember->GetSize());
		}

		// Value not found:
		return false;
	}
	template<typename T>
	bool UniformBuffer::SetValue(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const T& value)
	{
		const emberBufferLayout::BufferMember* pMember = m_bufferLayout.GetMember(arrayName);
		if (pMember != nullptr)
		{
			const emberBufferLayout::BufferMember* pSubMember = pMember->GetSubMember(arrayName + "[" + std::to_string(arrayIndex) + "]");
			if (pSubMember != nullptr)
			{
				const emberBufferLayout::BufferMember* pSubSubMember = pSubMember->GetSubMember(memberName);
				if (pSubSubMember != nullptr)
					return CheckAndUpdateData(value, pSubSubMember->GetOffset(), pSubSubMember->GetSize());
			}
		}

		// Value not found:
		return false;
	}
	template<typename T>
	bool UniformBuffer::SetValue(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const T& value)
	{
		const emberBufferLayout::BufferMember* pMember = m_bufferLayout.GetMember(arrayName);
		if (pMember != nullptr)
		{
			const emberBufferLayout::BufferMember* pSubMember = pMember->GetSubMember(arrayName + "[" + std::to_string(arrayIndex) + "]");
			if (pSubMember != nullptr)
			{
				const emberBufferLayout::BufferMember* pSubSubMember = pSubMember->GetSubMember(subArrayName);
				if (pSubSubMember != nullptr)
				{
					const emberBufferLayout::BufferMember* pSubSubSubMember = pSubSubMember->GetSubMember(subArrayName + "[" + std::to_string(subArrayIndex) + "]");
					if (pSubSubSubMember != nullptr)
						return CheckAndUpdateData(value, pSubSubSubMember->GetOffset(), pSubSubSubMember->GetSize());
				}
			}
		}

		// Value not found:
		return false;
	}



	// Templates for internal use:
	template<typename T>
	T UniformBuffer::GetData(uint32_t offset, uint32_t size) const
	{
		// Redirect bool to int:
		if constexpr (std::is_same<T, bool>::value)
		{
			int intValue = GetData<int>(offset, size);
			return static_cast<bool>(intValue);
		}

		T value;
		size_t dataSize = std::min(static_cast<size_t>(size), (sizeof(value)));
		std::memcpy(&value, m_hostData.data() + offset, dataSize);
		return value;
	}
	template<typename T>
	bool UniformBuffer::CheckAndUpdateData(const T& value, uint32_t offset, uint32_t size)
	{
		// Redirect bool to int:
		if constexpr (std::is_same<T, bool>::value)
		{
			int intValue = static_cast<int>(value);
			return CheckAndUpdateData<int>(intValue, offset, size);
		}

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
}