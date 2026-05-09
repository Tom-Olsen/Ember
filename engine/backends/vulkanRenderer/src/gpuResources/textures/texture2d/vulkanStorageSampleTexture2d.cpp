#include "vulkanStorageSampleTexture2d.h"
#include "assetLoader.h"
#include "vmaImage.h"
#include "vulkanAccessMask.h"
#include "vulkanContext.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include "vulkanStagingBuffer.h"



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Desctructor:
	StorageSampleTexture2d::StorageSampleTexture2d(Format format, int width, int height, void* data)
	{
		Init(format, width, height);
		if (data)
			SetData(data);
	}
	StorageSampleTexture2d::~StorageSampleTexture2d()
	{

	}



	// Movable:
	StorageSampleTexture2d::StorageSampleTexture2d(StorageSampleTexture2d&& other) noexcept = default;
	StorageSampleTexture2d& StorageSampleTexture2d::operator=(StorageSampleTexture2d&& other) noexcept = default;



	void StorageSampleTexture2d::SetData(void* data)
	{
		std::unique_ptr<StagingBuffer> pStagingBuffer = std::unique_ptr<StagingBuffer>(StageData(data));
		Upload(pStagingBuffer.get());
	}



	// Private methods:
	void StorageSampleTexture2d::Init(Format format, int width, int height)
	{
		if (!IsValidImageFormat(format))
			throw std::runtime_error("StorageSampleTexture2d::Init(...) failed. Unsupported format: " + std::to_string(static_cast<int>(format)));

		m_width = width;
		m_height = height;
		m_channels = GetChannelCount(format);
		m_format = format;
		m_vkDescriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

		// Define subresource range:
		VkImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.layerCount = 1;
		subresourceRange.levelCount = 1;	// no mipmapping for storage images.

		// Create image:
		VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		VkImageCreateFlags imageFlags = 0;
		VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
		DeviceQueue queue = Context::GetLogicalDevice()->GetTransferQueue();
		CreateImage(subresourceRange, m_format, usageFlags, imageFlags, memoryFlags, viewType, queue);

		NAME_VK_OBJECT(m_pImage->GetVkImage(), "StorageSampleTexture2d");
	}
	StagingBuffer* StorageSampleTexture2d::StageData(void* data)
	{
		// Copy: data -> pStagingBuffer
		uint64_t bufferSize = m_channels * m_width * m_height * BytesPerChannel(m_format);
		StagingBuffer* pStagingBuffer = new StagingBuffer(bufferSize);
		pStagingBuffer->SetData(data, bufferSize);
		return pStagingBuffer;
	}
	void StorageSampleTexture2d::Upload(StagingBuffer* pStagingBuffer)
    {
		// Transition 0: Layout: undefined->transfer, Queue: transfer
		{
			VkImageLayout newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			VkPipelineStageFlags2 srcStage = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
			VkPipelineStageFlags2 dstStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
			AccessMask srcAccessMask = AccessMasks::TopOfPipe::none;
			AccessMask dstAccessMask = AccessMasks::Transfer::transferWrite;
			m_pImage->TransitionLayout(newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
		}

		// Upload: pStagingBuffer -> texture
		pStagingBuffer->UploadToTexture(Context::GetLogicalDevice()->GetTransferQueue(), this, m_pImage->GetImageSubresourceRange().layerCount);

		// Transition 1: Layout: transfer->general, Queue: transfer->compute
		{
			VkImageLayout newLayout = VK_IMAGE_LAYOUT_GENERAL;
			VkPipelineStageFlags2 srcStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
			VkPipelineStageFlags2 dstStage = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
			AccessMask srcAccessMask = AccessMasks::Transfer::transferWrite;
			AccessMask dstAccessMask = AccessMasks::ComputeShader::memoryRead;
			m_pImage->TransitionLayout(newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
		}
    }
}