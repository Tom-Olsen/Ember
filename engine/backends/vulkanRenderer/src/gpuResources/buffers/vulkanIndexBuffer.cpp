#include "vulkanIndexBuffer.h"
#include "vmaBuffer.h"



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	IndexBuffer::IndexBuffer(uint32_t count, uint32_t elementSize)
	{
		m_count = count;
		m_elementSize = elementSize;
		m_size = m_count * m_elementSize;

		// Create buffer:
		BufferCreateInfo bufferInfo = {};
		bufferInfo.size = m_size;
		bufferInfo.usages = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		AllocationCreateInfo allocInfo = {};
		allocInfo.usages = MemoryUsages::auto_prefer_device;
		allocInfo.flags = 0;
		allocInfo.requiredFlags = 0;
		allocInfo.preferredFlags = 0;

		m_pBuffer = std::make_unique<VmaBuffer>(bufferInfo, allocInfo);
	}
	IndexBuffer::~IndexBuffer()
	{

	}



	// Movable:
	IndexBuffer::IndexBuffer(IndexBuffer&&) noexcept = default;
	IndexBuffer& IndexBuffer::operator=(IndexBuffer&&) noexcept = default;
}