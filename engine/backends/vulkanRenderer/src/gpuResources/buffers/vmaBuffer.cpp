#include "vmaBuffer.h"
#include "vmaAllocationCreateInfoToString.h"
#include "vulkanAllocationTracker.h"
#include "vulkanBufferCreateInfoToString.h"
#include "vulkanContext.h"
#include "vulkanGarbageCollector.h"
#include "vulkanMacros.h"
#include "vulkanMemoryAllocator.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Static private members:
	uint32_t VmaBuffer::s_index = 0;



	// Constructors/Destructor:
	VmaBuffer::VmaBuffer(const VkBufferCreateInfo& bufferInfo, const VmaAllocationCreateInfo& allocInfo)
	{
		m_bufferInfo = bufferInfo;
		m_allocationInfo = allocInfo;
		m_buffer = VK_NULL_HANDLE;
		m_allocation = nullptr;

		// Create Buffer:
		VkResult result = Context::GetMemoryAllocator()->CreateBuffer(m_bufferInfo, m_allocationInfo, m_bufferInfo.size, &m_buffer, &m_allocation);
		if (result != VK_SUCCESS)
		{
			LOG_CRITICAL("VmaBuffer::VmaBuffer failed. VkResult: {}, VkBufferCreateInfo: {}, VmaAllocationCreateInfo: {}", std::to_string(result), emberVulkanUtility::ToString(m_bufferInfo), ToString_VmaAllocationCreateInfo(m_allocationInfo));
			std::abort();
		}
		NAME_VK_OBJECT(m_buffer, "Buffer" + std::to_string(s_index));

		#ifdef VALIDATION_LAYERS_ACTIVE
		Context::GetAllocationTracker()->AddVmaBufferAllocation(m_buffer, m_allocation, std::to_string(s_index));
		#endif
		s_index++;
	}
	VmaBuffer::~VmaBuffer()
	{
		Cleanup();
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
	const VkBuffer& VmaBuffer::GetVkBuffer() const
	{
		return m_buffer;
	}
	const VmaAllocation& VmaBuffer::GetVmaAllocation() const
	{
		return m_allocation;
	}
	const VkBufferCreateInfo& VmaBuffer::GetBufferCreateInfo() const
	{
		return m_bufferInfo;
	}
	const VmaAllocationCreateInfo& VmaBuffer::GetAllocationCreateInfo() const
	{
		return m_allocationInfo;
	}
	uint64_t VmaBuffer::GetSize()
	{
		return m_bufferInfo.size;
	}
	void VmaBuffer::SetDebugName(const std::string& name)
	{
		NAME_VK_OBJECT(m_buffer, "Buffer_" + name);
		#ifdef VALIDATION_LAYERS_ACTIVE
		Context::GetAllocationTracker()->UpdateVmaBufferAllocationName(m_allocation, name);
		#endif
	}



	// Private methods:
	void VmaBuffer::Cleanup()
	{
		// Avoid double cleanup:
		if (m_buffer == VK_NULL_HANDLE && m_allocation == nullptr)
			return;

		// Capture current handles locally, then clear this object so destructor/moves are safe:
		VkBuffer buffer = m_buffer;
		VmaAllocation allocation = m_allocation;

		// Clear members so to avoid double cleanup:
		m_buffer = VK_NULL_HANDLE;
		m_allocation = nullptr;

		GarbageCollector::RecordGarbage([buffer, allocation]()
		{
				if (buffer != VK_NULL_HANDLE && allocation != nullptr)
					Context::GetMemoryAllocator()->DestroyBuffer(buffer, allocation);

			#ifdef VALIDATION_LAYERS_ACTIVE
			Context::GetAllocationTracker()->RemoveVmaBufferAllocation(allocation);
			#endif
		});
	}
	void VmaBuffer::MoveFrom(VmaBuffer& other) noexcept
	{
		m_buffer = other.m_buffer;
		m_allocation = other.m_allocation;
		m_bufferInfo = other.m_bufferInfo;
		m_allocationInfo = other.m_allocationInfo;

		other.m_buffer = VK_NULL_HANDLE;
		other.m_allocation = nullptr;
		other.m_bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		other.m_allocationInfo = {};
	}
}