#include "buffer.h"
#include "iBuffer.h"
#include "vulkanIndexBuffer.h"
#include "vulkanStorageBuffer.h"
#include "vulkanVertexBuffer.h"



namespace emberEngine
{
	// Constructor/Destructor:
	Buffer::Buffer(uint32_t count, uint32_t elementSize, const std::string& name, BufferUsage usage)
	{
		switch (usage)
		{
		case BufferUsage::index:
			m_pIBuffer = std::make_unique<vulkanRendererBackend::IndexBuffer>(count, elementSize, name);
			break;
		case BufferUsage::storage:
			m_pIBuffer = std::make_unique<vulkanRendererBackend::StorageBuffer>(count, elementSize, name);
			break;
		case BufferUsage::vertex:
			m_pIBuffer = std::make_unique<vulkanRendererBackend::VertexBuffer>(count, elementSize, name);
			break;
		}
	}
	Buffer::~Buffer();



	// Getters:
	std::string Buffer::GetName() const
	{
		return m_pIBuffer->GetName();
	}
	uint64_t Buffer::GetSize() const
	{
		return m_pIBuffer->GetSize();
	}
	uint32_t Buffer::GetCount() const
	{
		return m_pIBuffer->GetCount();
	}
	uint32_t Buffer::GetElementSize() const
	{
		return m_pIBuffer->GetElementSize();
	}



	// Upload/Download:
	void Buffer::Upload(const void* pSrc, uint64_t size)
	{
		m_pIBuffer->Upload(pSrc, size);
	}
	void Buffer::Download(void* pDst, uint64_t size)
	{
		m_pIBuffer->Download(pDst, size);
	}
}