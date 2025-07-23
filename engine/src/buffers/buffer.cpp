#include "buffer.h"
#include "emberMath.h"
#include "stagingBuffer.h"
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



	// Upload/Download:
	void Buffer::Upload(VkCommandBuffer commandBuffer, void* pSrc, uint64_t size)
	{
		size = math::Min(size, m_size);
		StagingBuffer stagingBuffer(size);
		stagingBuffer.SetData(pSrc, size);
		stagingBuffer.UploadToBuffer(commandBuffer, this);
	}
	void Buffer::Upload(void* pSrc, uint64_t size)
	{
		size = math::Min(size, m_size);
		StagingBuffer stagingBuffer(size);
		stagingBuffer.SetData(pSrc, size);
		stagingBuffer.UploadToBuffer(this, Context::logicalDevice.GetTransferQueue());
	}
	void Buffer::Download(VkCommandBuffer commandBuffer, void* pDst, uint64_t size)
	{
		size = math::Min(size, m_size);
		StagingBuffer stagingBuffer(size);
		stagingBuffer.DownloadFromBuffer(commandBuffer, this);
		stagingBuffer.GetData(pDst, size);
	}
	void Buffer::Download(void* pDst, uint64_t size)
	{
		size = math::Min(size, m_size);
		StagingBuffer stagingBuffer(size);
		stagingBuffer.DownloadFromBuffer(this, Context::logicalDevice.GetTransferQueue());
		stagingBuffer.GetData(pDst, size);
	}
}