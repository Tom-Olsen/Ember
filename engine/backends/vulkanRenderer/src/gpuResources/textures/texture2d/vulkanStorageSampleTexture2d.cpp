#include "vulkanStorageSampleTexture2d.h"
#include "assetLoader.h"
#include "logger.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanAccessMasks.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include "vulkanPipelineStages.h"
#include "vulkanStagingBuffer.h"



namespace vulkanRendererBackend
{
	// Constructor/Desctructor:
	StorageSampleTexture2d::StorageSampleTexture2d(const std::string& name, VkFormat format, int width, int height, void* data)
	{
		if (!IsValidImageFormat(format))
			throw std::runtime_error("StorageSampleTexture2d '" + name + "' uses unsuported format: " + std::to_string(static_cast<int>(format)));

		m_name = name;
		m_width = width;
		m_height = height;
        m_channels = GetChannelCount(format);
		m_format = format;
		m_descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

        std::unique_ptr<StagingBuffer> pStagingBuffer = std::unique_ptr<StagingBuffer>(Upload(data));
        Init(pStagingBuffer.get());

		NAME_VK_IMAGE(m_pImage->GetVkImage(), "StorageSampleTexture2d " + m_name);
	}
    StorageSampleTexture2d::StorageSampleTexture2d(const std::string& name, VkFormat format, const std::filesystem::path& path)
    {
		if (!IsValidImageFormat(format))
			throw std::runtime_error("StorageSampleTexture2d '" + name + "' uses unsuported format: " + std::to_string(static_cast<int>(format)));

        int channels = GetChannelCount(format);
        emberAssetLoader::Image imageAsset = emberAssetLoader::LoadImageFile(path, channels);

		m_name = name;
		m_width = imageAsset.width;
		m_height = imageAsset.height;
        m_channels = channels;
		m_format = format;
		m_descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        
		std::unique_ptr<StagingBuffer> pStagingBuffer = std::unique_ptr<StagingBuffer>(Upload((void*)imageAsset.pixels.data()));
		Init(pStagingBuffer.get());

		NAME_VK_IMAGE(m_pImage->GetVkImage(), "StorageSampleTexture2d " + m_name);
    }
	StorageSampleTexture2d::~StorageSampleTexture2d()
	{

	}



	// Public methods:
	void StorageSampleTexture2d::RecordGpuCommands(VkCommandBuffer& transferCommandBuffer, VkCommandBuffer& graphicsCommandBuffer, StagingBuffer* pStagingBuffer)
	{
		LOG_WARN("StorageSampleTexture2d::RecordGpuCommands(...) not implemented yet.");
	}



	// Private methods:
	StagingBuffer* StorageSampleTexture2d::Upload(void* data)
	{
		// Copy: data -> pStagingBuffer
		uint64_t bufferSize = m_channels * m_width * m_height * BytesPerChannel(m_format);
		StagingBuffer* pStagingBuffer = new StagingBuffer(bufferSize, m_name);
		pStagingBuffer->SetData(data, bufferSize);

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
		DeviceQueue queue = Context::logicalDevice.GetTransferQueue();
		CreateImage(subresourceRange, m_format, usageFlags, imageFlags, memoryFlags, viewType, queue);

		return pStagingBuffer;
	}
	void StorageSampleTexture2d::Init(StagingBuffer* pStagingBuffer)
    {
		// Transition 0: Layout: undefined->transfer, Queue: transfer
		{
			VkImageLayout newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			VkPipelineStageFlags2 srcStage = pipelineStage::topOfPipe;
			VkPipelineStageFlags2 dstStage = pipelineStage::transfer;
			VkAccessFlags2 srcAccessMask = accessMask::topOfPipe::none;
			VkAccessFlags2 dstAccessMask = accessMask::transfer::transferWrite;
			m_pImage->TransitionLayout(newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
		}

		// Upload: pStagingBuffer -> image
		pStagingBuffer->UploadToImage(Context::logicalDevice.GetTransferQueue(), m_pImage.get(), m_pImage->GetSubresourceRange().layerCount);

		// Transition 1: Layout: transfer->general, Queue: transfer->compute
		{
			VkImageLayout newLayout = VK_IMAGE_LAYOUT_GENERAL;
			VkPipelineStageFlags2 srcStage = pipelineStage::transfer;
			VkPipelineStageFlags2 dstStage = pipelineStage::computeShader;
			VkAccessFlags2 srcAccessMask = accessMask::transfer::transferWrite;
			VkAccessFlags2 dstAccessMask = accessMask::computeShader::memoryRead;
			m_pImage->TransitionLayout(newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
		}
    }
}