#include "vulkanBufferHandle.h"
#include "logger.h"
#include "vulkanBuffer.h"
#include "vulkanGpuResourceRegistry.h"
#include <stdexcept>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	BufferHandle::BufferHandle()
	{

	}
	BufferHandle::BufferHandle(const Buffer& buffer)
		: m_resourceHandle(buffer.m_registrationHandle)
	{
		if (!m_resourceHandle.IsValid())
			throw std::runtime_error("BufferHandle::BufferHandle(...) failed. Buffer has been moved from.");
	}
	BufferHandle::~BufferHandle()
	{

	}



	// Getters:
	Buffer* BufferHandle::Get() const
	{
		Buffer* pBuffer = Buffer::s_resourceRegistry.TryGet(m_resourceHandle);
		if (pBuffer == nullptr)
			throw std::runtime_error("BufferHandle::Get() failed. Buffer is invalid or expired.");
		return pBuffer;
	}
	Buffer* BufferHandle::TryGet() const
	{
		Buffer* pBuffer = Buffer::s_resourceRegistry.TryGet(m_resourceHandle);
		if (pBuffer == nullptr)
			LOG_WARN("BufferHandle::TryGet() failed. Buffer is invalid or expired.");
		return pBuffer;
	}
	bool BufferHandle::IsValid() const
	{
		return Buffer::s_resourceRegistry.TryGet(m_resourceHandle) != nullptr;
	}



	// Comparison operators:
	bool BufferHandle::operator==(const BufferHandle& other) const
	{
		return m_resourceHandle == other.m_resourceHandle;
	}
	bool BufferHandle::operator!=(const BufferHandle& other) const
	{
		return !(*this == other);
	}
}