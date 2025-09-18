#include "vulkanStagingBuffer.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanAccessMasks.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include "vulkanPipelineStages.h"
#include "vulkanSingleTimeCommand.h"
#include "vulkanTexture.h"
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

		// Query memory type properties to detect coherency:
		VkMemoryPropertyFlags propertyFlags = 0;
		vmaGetMemoryTypeProperties(Context::GetVmaAllocator(), allocInfoOut.memoryType, &propertyFlags);
		isCoherent = (propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0;
	}
	StagingBuffer::~StagingBuffer()
	{

	}



	// Public methods:
	// Getters:
	void StagingBuffer::GetData(void* pDst, uint64_t size, uint64_t offset) const
	{
		if (!pDst || size == 0 || offset >= m_size)
			return;

		// Invalidate if not coherent:
		if (!isCoherent)
			vmaInvalidateAllocation(Context::GetVmaAllocator(), m_pBuffer->GetVmaAllocation(), offset, size);

		size = std::min(size, m_size - offset);
		std::memcpy(pDst, static_cast<const char*>(m_pDeviceData) + offset, size);
	}

	// Setters:
	void StagingBuffer::SetData(const void* pSrc, uint64_t size, uint64_t offset)
	{
		if (!pSrc || size == 0 || offset >= m_size)
			return;

		size = std::min(size, m_size - offset);
		std::memcpy(static_cast<char*>(m_pDeviceData) + offset, pSrc, size);

		// Flush if not coherent:
		if (!isCoherent)
			vmaFlushAllocation(Context::GetVmaAllocator(), m_pBuffer->GetVmaAllocation(), offset, size);
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
	void StagingBuffer::UploadToTexture(VkCommandBuffer vkCommandBuffer, Texture* pDstTexture, uint64_t layerCount)
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
		region.imageExtent = pDstTexture->GetVmaImage()->GetExtent();

		vkCmdCopyBufferToImage(
			vkCommandBuffer,
			m_pBuffer->GetVkBuffer(),
			pDstTexture->GetVmaImage()->GetVkImage(),
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region);
	}
	void StagingBuffer::UploadToTexture(const DeviceQueue& queue, Texture* pDstTexture, uint64_t layerCount)
	{
		VkCommandBuffer vkCommandBuffer = SingleTimeCommand::BeginCommand(queue);
		UploadToTexture(vkCommandBuffer, pDstTexture, layerCount);
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
	void StagingBuffer::DownloadFromTexture(VkCommandBuffer commandBuffer, Texture* pSrcTexture)
    {
        // Cache original layout:
        VkImageLayout originalLayout = pSrcTexture->GetVmaImage()->GetLayout();

        // Ember::ToDo: track stage and access usage of textures and use them here!
		// Transition 0: Layout: original->srcTransfer, Queue: transfer
        {
		    VkImageLayout newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		    VkPipelineStageFlags2 srcStage = pipelineStage::bottomOfPipe;   // use tracked stage here.
		    VkPipelineStageFlags2 dstStage = pipelineStage::copy;
		    VkAccessFlags2 srcAccessMask = accessMask::bottomOfPipe::none;  // use tracked access here.
		    VkAccessFlags2 dstAccessMask = accessMask::copy::transferRead;
            pSrcTexture->GetVmaImage()->TransitionLayout(commandBuffer, newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
        }

        // Copy image to staging buffer:
        {
            VkBufferImageCopy2 region = { VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2 };
            region.bufferOffset       = 0;
            region.bufferRowLength    = 0;  // 0 = tightly packed
            region.bufferImageHeight  = 0;  // 0 = tightly packed
            region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.mipLevel       = 0;
            region.imageSubresource.baseArrayLayer = 0;
            region.imageSubresource.layerCount     = 1;
            region.imageOffset = { 0, 0, 0 };
            region.imageExtent = { pSrcTexture->GetWidth(), pSrcTexture->GetHeight(), pSrcTexture->GetDepth() };

            VkCopyImageToBufferInfo2 copyInfo = { VK_STRUCTURE_TYPE_COPY_IMAGE_TO_BUFFER_INFO_2 };
            copyInfo.srcImage = pSrcTexture->GetVmaImage()->GetVkImage();
            copyInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            copyInfo.dstBuffer = m_pBuffer->GetVkBuffer();
            copyInfo.regionCount = 1;
            copyInfo.pRegions = &region;

            vkCmdCopyImageToBuffer2(commandBuffer, &copyInfo);
        }

		// Transition 1: Layout: srcTransfer->original, Queue: transfer
        {
		    VkImageLayout newLayout = originalLayout;
		    VkPipelineStageFlags2 srcStage = pipelineStage::copy;
		    VkPipelineStageFlags2 dstStage = pipelineStage::bottomOfPipe;   // use tracked stage here.
		    VkAccessFlags2 srcAccessMask = accessMask::copy::transferRead;
		    VkAccessFlags2 dstAccessMask = accessMask::bottomOfPipe::none;  // use access stage here.
            pSrcTexture->GetVmaImage()->TransitionLayout(commandBuffer, newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
        }
    }
	void StagingBuffer::DownloadFromTexture(Texture* pSrcTexture, const DeviceQueue& queue)
    {
		VkCommandBuffer vkCommandBuffer = SingleTimeCommand::BeginCommand(queue);
		DownloadFromTexture(vkCommandBuffer, pSrcTexture);
		SingleTimeCommand::EndCommand(queue);
    }
}