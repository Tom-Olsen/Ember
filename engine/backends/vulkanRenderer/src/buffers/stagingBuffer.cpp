#include "stagingBuffer.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include "vulkanSingleTimeCommand.h"
#include <cstring>



namespace vulkanRendererBackend
{
	// Constructor/Destructor:
	StagingBuffer::StagingBuffer(uint64_t size, std::string name)
	{
		m_size = size;
		m_name = name;

		// Create buffer:
		VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.size = m_size;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
		allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
		allocInfo.requiredFlags = 0;
		allocInfo.preferredFlags = 0;

		m_pBuffer = std::make_unique<VmaBuffer>("stagingBuffer", bufferInfo, allocInfo);
		NAME_VK_BUFFER(m_pBuffer->GetVkBuffer(), "stagingBuffer " + m_name);

		// Get mapped deviceData pointer:
		VmaAllocationInfo allocInfoOut;
		vmaGetAllocationInfo(Context::GetVmaAllocator(), m_pBuffer->GetVmaAllocation(), &allocInfoOut);
		m_pDeviceData = allocInfoOut.pMappedData;
	}
	StagingBuffer::~StagingBuffer()
	{

	}



	// Public methods:
	// Getters:
	void StagingBuffer::GetData(void* pDst, uint64_t size) const
	{
		if (!pDst || size == 0)
			return;

		size = std::min(size, m_size);
		std::memcpy(pDst, m_pDeviceData, static_cast<size_t>(size));
	}

	void StagingBuffer::GetData(void* pDst, uint64_t size, uint64_t offset) const
	{
		if (!pDst || size == 0 || offset >= m_size)
			return;

		size = std::min(size, m_size - offset);
		std::memcpy(pDst, static_cast<const char*>(m_pDeviceData) + offset, size);
	}

	// Setters:
	void StagingBuffer::SetData(const void* pSrc, uint64_t size)
	{
		if (!pSrc || size == 0)
			return;

		size = std::min(size, m_size);
		memcpy(m_pDeviceData, pSrc, static_cast<size_t>(size));
	}
	void StagingBuffer::SetData(const void* pSrc, uint64_t size, uint64_t offset)
	{
		if (!pSrc || size == 0 || offset >= m_size)
			return;

		size = std::min(size, m_size - offset);
		std::memcpy(static_cast<char*>(m_pDeviceData) + offset, pSrc, size);
	}



	// Upload:
	void StagingBuffer::UploadToBuffer(VkCommandBuffer vkCommandBuffer, Buffer* pDstBuffer)
	{
		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = pDstBuffer->GetSize();

		vkCmdCopyBuffer(
			vkCommandBuffer,
			m_pBuffer->GetVkBuffer(),
			pDstBuffer->GetVmaBuffer()->GetVkBuffer(),
			1,
			&copyRegion);
	}
	void StagingBuffer::UploadToBuffer(Buffer* pDstBuffer, const DeviceQueue& queue)
	{
		VkCommandBuffer vkCommandBuffer = SingleTimeCommand::BeginCommand(queue);
		UploadToBuffer(vkCommandBuffer, pDstBuffer);
		SingleTimeCommand::EndCommand(queue);
	}
	void StagingBuffer::UploadToImage(VkCommandBuffer vkCommandBuffer, VmaImage* pDstImage, uint64_t layerCount)
	{
		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = layerCount;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = pDstImage->GetExtent();

		vkCmdCopyBufferToImage(
			vkCommandBuffer,
			m_pBuffer->GetVkBuffer(),
			pDstImage->GetVkImage(),
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region);
	}
	void StagingBuffer::UploadToImage(const DeviceQueue& queue, VmaImage* pDstImage, uint64_t layerCount)
	{
		VkCommandBuffer vkCommandBuffer = SingleTimeCommand::BeginCommand(queue);
		UploadToImage(vkCommandBuffer, pDstImage, layerCount);
		SingleTimeCommand::EndCommand(queue);
	}

	// Download:
	void StagingBuffer::DownloadFromBuffer(VkCommandBuffer vkCommandBuffer, Buffer* pSrcBuffer)
	{
		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = std::min(m_size, pSrcBuffer->GetSize());

		vkCmdCopyBuffer(
			vkCommandBuffer,
			pSrcBuffer->GetVmaBuffer()->GetVkBuffer(),
			m_pBuffer->GetVkBuffer(),
			1,
			&copyRegion);
	}
	void StagingBuffer::DownloadFromBuffer(Buffer* pSrcBuffer, const DeviceQueue& queue)
	{
		VkCommandBuffer vkCommandBuffer = SingleTimeCommand::BeginCommand(queue);
		DownloadFromBuffer(vkCommandBuffer, pSrcBuffer);
		SingleTimeCommand::EndCommand(queue);
	}
}