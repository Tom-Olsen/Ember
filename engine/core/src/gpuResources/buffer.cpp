#include "buffer.h"
#include "iBuffer.h"
#include "renderer.h"



namespace emberEngine
{
	// Private methods:
	emberBackendInterface::IBuffer* Buffer::GetInterfaceHandle()
	{
		return m_pIBuffer.get();
	}



	// Public methods:
	// Constructor/Destructor:
	Buffer::Buffer()
	{
		m_pIBuffer = nullptr;
	}
	Buffer::Buffer(uint32_t count, uint32_t elementSize, const std::string& name, emberCommon::BufferUsage usage)
	{
		m_pIBuffer = std::unique_ptr<emberBackendInterface::IBuffer>(Renderer::CreateBuffer(count, elementSize, name, usage));
	}
	Buffer::~Buffer()
	{

	}



	// Movable:
	Buffer::Buffer(Buffer&& other) noexcept = default;
	Buffer& Buffer::operator=(Buffer&& other) noexcept = default;



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
	bool Buffer::IsValid() const
	{
		return m_pIBuffer != nullptr;
	}
}