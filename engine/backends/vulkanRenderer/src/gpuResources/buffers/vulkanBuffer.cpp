#include "vulkanBuffer.h"
#include "vmaBuffer.h"
#include "vulkanContext.h"
#include "vulkanStagingBuffer.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Virtual Destructor:
	Buffer::~Buffer()
	{
		Context::WaitDeviceIdle();
	}



	// Public Methods:
	// Getters:
	std::string Buffer::GetName() const
	{
		return m_name;
	}
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
		return m_pBuffer.get();
	}



	// Upload/Download:
	void Buffer::Upload(VkCommandBuffer vkCommandBuffer, void* pSrc, uint64_t size)
	{
		size = std::min(size, m_size);
		StagingBuffer stagingBuffer(size, "tempA Buffer::Upload(...)");
		stagingBuffer.SetData(pSrc, size);
		stagingBuffer.UploadToBuffer(vkCommandBuffer, this);
	}
	void Buffer::Upload(const void* pSrc, uint64_t size)
	{
		size = std::min(size, m_size);
		StagingBuffer stagingBuffer(size, "tempB Buffer::Upload(...)");
		stagingBuffer.SetData(pSrc, size);
		stagingBuffer.UploadToBuffer(this, Context::logicalDevice.GetTransferQueue());
	}
	void Buffer::Download(VkCommandBuffer vkCommandBuffer, void* pDst, uint64_t size)
	{
		size = std::min(size, m_size);
		StagingBuffer stagingBuffer(size, "tempA Buffer::Download(...)");
		stagingBuffer.DownloadFromBuffer(vkCommandBuffer, this);
		stagingBuffer.GetData(pDst, size);
	}
	void Buffer::Download(void* pDst, uint64_t size)
	{
		size = std::min(size, m_size);
		StagingBuffer stagingBuffer(size, "tempB Buffer::Download(...)");
		stagingBuffer.DownloadFromBuffer(this, Context::logicalDevice.GetTransferQueue());
		stagingBuffer.GetData(pDst, size);
	}
}