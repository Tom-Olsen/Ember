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
		bufferInfo.usages = BufferUsageFlags::index_buffer_bit | BufferUsageFlags::transfer_dst_bit;
		bufferInfo.sharingMode = SharingModes::exclusive;

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