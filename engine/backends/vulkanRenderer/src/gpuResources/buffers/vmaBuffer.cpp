#include "vmaBuffer.h"
#include "vk_mem_alloc.h"
#include "vulkanContext.h"
#include "vulkanGarbageCollector.h"
#include "vulkanMacros.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Static private members:
	uint32_t VmaBuffer::s_index = 0;



	// Constructors/Destructor:
	VmaBuffer::VmaBuffer(const std::string& name, const BufferCreateInfo& bufferInfo, const AllocationCreateInfo& allocInfo)
	{
		m_name = name + std::to_string(s_index);
		s_index++;
		m_bufferInfo = bufferInfo;
		m_allocationInfo = allocInfo;
		m_buffer = VK_NULL_HANDLE;
		m_allocation = nullptr;

		// Convert BufferCreateInfo -> VkBufferCreateInfo:
		VkBufferCreateInfo vkBufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		vkBufferInfo.pNext = m_bufferInfo.pNext;
		vkBufferInfo.flags = m_bufferInfo.flags;
		vkBufferInfo.size = m_bufferInfo.size;
		vkBufferInfo.usage = m_bufferInfo.usages;
		vkBufferInfo.sharingMode = static_cast<VkSharingMode>(m_bufferInfo.sharingMode);
		vkBufferInfo.queueFamilyIndexCount = m_bufferInfo.queueFamilyIndexCount;
		vkBufferInfo.pQueueFamilyIndices = m_bufferInfo.pQueueFamilyIndices;

		// Convert AllocationCreateInfo -> VmaAllocationCreateInfo:
		VmaAllocationCreateInfo vmaAllocationInfo = {};
		vmaAllocationInfo.flags = m_allocationInfo.flags;
		vmaAllocationInfo.usage = static_cast<VmaMemoryUsage>(m_allocationInfo.usages);
		vmaAllocationInfo.requiredFlags = m_allocationInfo.requiredFlags;
		vmaAllocationInfo.preferredFlags = m_allocationInfo.preferredFlags;
		vmaAllocationInfo.memoryTypeBits = m_allocationInfo.memoryTypeBits;
		vmaAllocationInfo.pool = m_allocationInfo.pool;
		vmaAllocationInfo.pUserData = m_allocationInfo.pUserData;
		vmaAllocationInfo.priority = m_allocationInfo.priority;

		// Create Buffer:
		VKA(vmaCreateBuffer(Context::GetVmaAllocator(), &vkBufferInfo, &vmaAllocationInfo, &m_buffer, &m_allocation, nullptr));

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
	const BufferCreateInfo& VmaBuffer::GetBufferCreateInfo() const
	{
		return m_bufferInfo;
	}
	const AllocationCreateInfo& VmaBuffer::GetAllocationCreateInfo() const
	{
		return m_allocationInfo;
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
		GarbageCollector::RecordGarbage([buffer, allocation]()
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
		m_allocationInfo = other.m_allocationInfo;

		other.m_name = "";
		other.m_buffer = VK_NULL_HANDLE;
		other.m_allocation = nullptr;
		other.m_bufferInfo = {};
		other.m_allocationInfo = {};
	}
}