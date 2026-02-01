#include "vulkanVertexBuffer.h"
#include "vmaBuffer.h"



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	VertexBuffer::VertexBuffer(uint32_t count, uint32_t elementSize)
	{
		m_count = count;
		m_elementSize = elementSize;
		m_size = m_count * m_elementSize;

		// Create buffer:
		BufferCreateInfo bufferInfo = {};
		bufferInfo.size = m_size;
		bufferInfo.usages = BufferUsageFlags::vertex_buffer_bit | BufferUsageFlags::transfer_dst_bit;
		bufferInfo.sharingMode = SharingModes::exclusive;

		AllocationCreateInfo allocInfo = {};
		allocInfo.usages = MemoryUsages::auto_prefer_device;
		allocInfo.flags = 0;
		allocInfo.requiredFlags = 0;
		allocInfo.preferredFlags = 0;

		m_pBuffer = std::make_unique<VmaBuffer>(bufferInfo, allocInfo);
	}
	VertexBuffer::~VertexBuffer()
	{

	}



	// Movable:
	VertexBuffer::VertexBuffer(VertexBuffer&&) noexcept = default;
	VertexBuffer& VertexBuffer::operator=(VertexBuffer&&) noexcept = default;
}