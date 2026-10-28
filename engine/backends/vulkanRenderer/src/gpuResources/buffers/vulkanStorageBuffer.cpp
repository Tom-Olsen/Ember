#include "vulkanStorageBuffer.h"
#include "vmaBuffer.h"
#include <stdexcept>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	StorageBuffer::StorageBuffer(uint32_t count, uint32_t elementSize)
	{
		// Error handling:
		if (count == 0)
			throw std::invalid_argument("StorageBuffer requires a non-zero count.");
		if (elementSize == 0)
			throw std::invalid_argument("StorageBuffer requires a non-zero elementSize.");
		
		m_count = count;
		m_elementSize = elementSize;
		m_size = static_cast<uint64_t>(m_count) * m_elementSize;

		// Create buffer:
		VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.size = m_size;
		bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		allocInfo.flags = 0;
		allocInfo.requiredFlags = 0;
		allocInfo.preferredFlags = 0;

		m_pBuffer = std::make_unique<VmaBuffer>(bufferInfo, allocInfo);
	}
	StorageBuffer::~StorageBuffer()
	{

	}



	// Movable:
	StorageBuffer::StorageBuffer(StorageBuffer&&) noexcept = default;
	StorageBuffer& StorageBuffer::operator=(StorageBuffer&&) noexcept = default;
}