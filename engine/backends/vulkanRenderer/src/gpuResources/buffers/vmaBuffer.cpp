#include "vmaBuffer.h"
#include "vulkanContext.h"
#include "vulkanGarbageCollector.h"
#include "vk_mem_alloc.h"
#include "vulkanMacros.h"



namespace vulkanRendererBackend
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
		m_buffer = VK_NULL_HANDLE;
		m_allocation = nullptr;
		VKA(vmaCreateBuffer(Context::GetVmaAllocator(), &m_bufferInfo, &m_allocInfo, &m_buffer, &m_allocation, nullptr));

		#ifdef VALIDATION_LAYERS_ACTIVE
		Context::allocationTracker.AddVmaBuffer(this);
		#endif
	}
	VmaBuffer::~VmaBuffer()
	{
		Cleanup();
		#ifdef VALIDATION_LAYERS_ACTIVE
		Context::allocationTracker.RemoveVmaBuffer(this);
		#endif
	}



	// Move semantics:
	VmaBuffer::VmaBuffer(VmaBuffer&& other) noexcept
	{
		MoveFrom(other);
	}
	VmaBuffer& VmaBuffer::operator=(VmaBuffer&& other) noexcept
	{
		if (this != &other)
		{
			Cleanup();
			MoveFrom(other);
		}
		return *this;
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



	// Private methods:
	void VmaBuffer::Cleanup()
	{
		VkBuffer buffer = m_buffer;
		VmaAllocation allocation = m_allocation;
		GarbageCollector::RecordCleanup([buffer, allocation]()
		{
			vmaDestroyBuffer(Context::GetVmaAllocator(), buffer, allocation);
		});
	}
	void VmaBuffer::MoveFrom(VmaBuffer& other) noexcept
	{
		m_name = other.m_name;
		m_buffer = other.m_buffer;
		m_allocation = other.m_allocation;
		m_bufferInfo = other.m_bufferInfo;
		m_allocInfo = other.m_allocInfo;

		other.m_name = "";
		other.m_buffer = VK_NULL_HANDLE;
		other.m_allocation = nullptr;
		other.m_bufferInfo = {};
		other.m_allocInfo = {};
	}
}