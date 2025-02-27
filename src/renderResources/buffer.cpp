#include "buffer.h"
#include "vulkanContext.h"
#include "vmaBuffer.h"
#include <vulkan/vulkan.h>



namespace emberEngine
{
	// Virtual Destructor:
	Buffer::~Buffer()
	{
		m_pContext->WaitDeviceIdle();
	}

	// Public Methods:
	uint32_t Buffer::GetSize() const
	{
		return m_size;
	}
	const VmaBuffer* const Buffer::GetVmaBuffer() const
	{
		return m_buffer.get();
	}
}