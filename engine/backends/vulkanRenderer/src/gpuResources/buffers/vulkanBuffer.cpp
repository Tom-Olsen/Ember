#include "vulkanBuffer.h"
#include "vmaBuffer.h"
#include "vulkanContext.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include "vulkanStagingBuffer.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public Methods:
	// Constructor/Destructor:
	Buffer::Buffer()
	{

	}
	Buffer::~Buffer()
	{

	}



	// Movable:
	Buffer::Buffer(Buffer&&) noexcept = default;
	Buffer& Buffer::operator=(Buffer&&) noexcept = default;



	// Getters:
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



	// Data transfer:
	void Buffer::Upload(const void* pSrc, uint64_t size)
	{
		size = std::min(size, m_size);
		StagingBuffer stagingBuffer(size, "tempB Buffer::Upload(...)");
		stagingBuffer.SetData(pSrc, size);
		stagingBuffer.UploadToBuffer(this, Context::GetLogicalDevice()->GetTransferQueue());
	}
	void Buffer::Download(void* pDst, uint64_t size)
	{
		size = std::min(size, m_size);
		StagingBuffer stagingBuffer(size, "tempB Buffer::Download(...)");
		stagingBuffer.DownloadFromBuffer(this, Context::GetLogicalDevice()->GetTransferQueue());
		stagingBuffer.GetData(pDst, size);
	}



	// Debugging:
	void Buffer::SetDebugName(const std::string& name)
	{
		NAME_VK_OBJECT(m_pBuffer->GetVkBuffer(), name);
	}



	// Backend only:
	void Buffer::Upload(VkCommandBuffer vkCommandBuffer, void* pSrc, uint64_t size)
	{
		size = std::min(size, m_size);
		StagingBuffer stagingBuffer(size, "tempA Buffer::Upload(...)");
		stagingBuffer.SetData(pSrc, size);
		stagingBuffer.UploadToBuffer(vkCommandBuffer, this);
	}
	void Buffer::Download(VkCommandBuffer vkCommandBuffer, void* pDst, uint64_t size)
	{
		size = std::min(size, m_size);
		StagingBuffer stagingBuffer(size, "tempA Buffer::Download(...)");
		stagingBuffer.DownloadFromBuffer(vkCommandBuffer, this);
		stagingBuffer.GetData(pDst, size);
	}
}