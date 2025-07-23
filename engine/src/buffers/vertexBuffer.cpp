#include "vertexBuffer.h"
#include "vmaBuffer.h"



namespace emberEngine
{
	using namespace vulkanBackend;



	// Constructor/Destructor:
	VertexBuffer::VertexBuffer(uint32_t count, uint32_t elementSize)
	{
		m_count = count;
		m_elementSize = elementSize;
		m_size = m_count * m_elementSize;

		// Create buffer:
		VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.size = m_size;
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		allocInfo.flags = 0;
		allocInfo.requiredFlags = 0;
		allocInfo.preferredFlags = 0;

		m_buffer = std::make_unique<VmaBuffer>("vertexBuffer", bufferInfo, allocInfo);
	}
	VertexBuffer::~VertexBuffer()
	{

	}
}