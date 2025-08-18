#include "indexBuffer.h"
#include "vmaBuffer.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"



namespace emberEngine
{
	using namespace vulkanBackend;



	// Constructor/Destructor:
	IndexBuffer::IndexBuffer(uint32_t count, uint32_t elementSize, std::string name)
	{
		m_count = count;
		m_elementSize = elementSize;
		m_size = m_count * m_elementSize;
		m_name = name;

		// Create buffer:
		VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.size = m_size;
		bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		allocInfo.flags = 0;
		allocInfo.requiredFlags = 0;
		allocInfo.preferredFlags = 0;

		m_pBuffer = std::make_unique<VmaBuffer>("indexBuffer", bufferInfo, allocInfo);
		NAME_VK_BUFFER(m_pBuffer->GetVkBuffer(), "indexBuffer " + m_name);
	}
	IndexBuffer::~IndexBuffer()
	{

	}
}