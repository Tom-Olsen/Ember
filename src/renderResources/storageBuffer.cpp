#include "storageBuffer.h"
#include "vulkanContext.h"
#include "vmaBuffer.h"
#include <vulkan/vulkan.h>



namespace emberEngine
{
	// Constructor/Destructor:
	StorageBuffer::StorageBuffer(VulkanContext* pContext, uint32_t count, uint32_t elementSize)
	{
		m_count = count;
		m_elementSize = elementSize;
		m_size = m_count * m_elementSize;
		m_pContext = pContext;

		// Create buffer:
		VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.size = m_size;
		bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		allocInfo.flags = 0;
		allocInfo.requiredFlags = 0;
		allocInfo.preferredFlags = 0;

		m_buffer = std::make_unique<VmaBuffer>(m_pContext, bufferInfo, allocInfo);
	}
	StorageBuffer::~StorageBuffer()
	{

	}



	// Public methdos:
	uint32_t StorageBuffer::GetCount() const
	{
		return m_count;
	}
	uint32_t StorageBuffer::GetElementSize() const
	{
		return m_elementSize;
	}
}