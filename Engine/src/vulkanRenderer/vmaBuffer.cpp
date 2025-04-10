#include "vmaBuffer.h"
#include "logger.h"
#include "vmaImage.h"
#include "vulkanCommand.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"



namespace emberEngine
{
	// Static private members:
	uint32_t VmaBuffer::s_index = 0;



	// Constructors/Destructor:
	VmaBuffer::VmaBuffer(const std::string& name, const VkBufferCreateInfo& bufferInfo, const VmaAllocationCreateInfo& allocInfo)
	{
		m_name = name + std::to_string(s_index);
		s_index++;
		m_bufferInfo = bufferInfo;
		m_allocInfo = allocInfo;
		VKA(vmaCreateBuffer(VulkanContext::GetVmaAllocator(), &m_bufferInfo, &m_allocInfo, &m_buffer, &m_allocation, nullptr));

		#ifdef VALIDATION_LAYERS_ACTIVE
		VulkanContext::pAllocationTracker->AddVmaBuffer(this);
		#endif
	}
	VmaBuffer::VmaBuffer(VmaBuffer&& other) noexcept
		: m_buffer(other.m_buffer),
		m_allocation(other.m_allocation),
		m_bufferInfo(other.m_bufferInfo),
		m_allocInfo(other.m_allocInfo)
	{
		other.m_buffer = VK_NULL_HANDLE;
		other.m_allocation = nullptr;
	}
	VmaBuffer& VmaBuffer::operator=(VmaBuffer&& other) noexcept
	{
		if (this != &other)
		{
			// Free current resources
			m_buffer = VK_NULL_HANDLE;
			m_allocation = nullptr;

			// Move data from other
			m_name = other.m_name;
			m_buffer = other.m_buffer;
			m_allocation = other.m_allocation;
			m_bufferInfo = other.m_bufferInfo;
			m_allocInfo = other.m_allocInfo;

			// Reset other
			other.m_buffer = VK_NULL_HANDLE;
			other.m_allocation = nullptr;
		}
		return *this;
	}
	VmaBuffer::~VmaBuffer()
	{
		vmaDestroyBuffer(VulkanContext::GetVmaAllocator(), m_buffer, m_allocation);

		#ifdef VALIDATION_LAYERS_ACTIVE
		VulkanContext::pAllocationTracker->RemoveVmaBuffer(this);
		#endif
	}



	// Public methods:
	// Getters:
	const std::string& VmaBuffer::GetName() const
	{
		return m_name;
	}
	const VkBuffer& VmaBuffer::GetVkBuffer() const
	{
		return m_buffer;
	}
	const VmaAllocation& VmaBuffer::GetVmaAllocation() const
	{
		return m_allocation;
	}
	const VkBufferCreateInfo& VmaBuffer::GetVkBufferCreateInfo() const
	{
		return m_bufferInfo;
	}
	const VmaAllocationCreateInfo& VmaBuffer::GetVmaAllocationCreateInfo() const
	{
		return m_allocInfo;
	}
	uint64_t VmaBuffer::GetSize()
	{
		return m_bufferInfo.size;
	}
}