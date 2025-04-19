#include "buffer.h"
#include "vulkanContext.h"
#include "vmaBuffer.h"
#include <vulkan/vulkan.h>



namespace emberEngine
{
	using namespace vulkanBackend;



	// Virtual Destructor:
	Buffer::~Buffer()
	{
		Context::WaitDeviceIdle();
	}

	// Public Methods:
	uint64_t Buffer::GetSize() const
	{
		return m_size;
	}
	uint32_t Buffer::GetCount() const
	{
		return m_count;
	}
	uint32_t Buffer::GetElementSize() const
	{
		return m_elementSize;
	}
	VmaBuffer* const Buffer::GetVmaBuffer() const
	{
		return m_buffer.get();
	}
}