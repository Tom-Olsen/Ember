#include "vulkanVertexBuffer.h"
#include "vmaBuffer.h"
#include "vulkanMacros.h"



namespace vulkanRendererBackend
{
	// Constructor/Destructor:
	VertexBuffer::VertexBuffer(uint32_t count, uint32_t elementSize, std::string name)
	{
		m_count = count;
		m_elementSize = elementSize;
		m_size = m_count * m_elementSize;
		m_name = name;

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

		m_pBuffer = std::make_unique<VmaBuffer>("vertexBuffer", bufferInfo, allocInfo);
		NAME_VK_BUFFER(m_pBuffer->GetVkBuffer(), "vertexBuffer " + m_name);
	}
	VertexBuffer::~VertexBuffer()
	{

	}
}