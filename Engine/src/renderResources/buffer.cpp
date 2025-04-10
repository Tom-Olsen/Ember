#include "buffer.h"
#include "vulkanContext.h"
#include "vmaBuffer.h"
#include <vulkan/vulkan.h>



namespace emberEngine
{
	// Virtual Destructor:
	Buffer::~Buffer()
	{
		VulkanContext::WaitDeviceIdle();
	}

	// Public Methods:
	uint64_t Buffer::GetSize() const
	{
		return m_size;
	}
	VmaBuffer* const Buffer::GetVmaBuffer() const
	{
		return m_buffer.get();
	}
}