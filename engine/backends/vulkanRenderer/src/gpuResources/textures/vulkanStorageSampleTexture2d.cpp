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
		m_name = name;
		m_width = width;
		m_height = height;
        m_channels = GetChannelCount(format);
		m_format = format;
		m_descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

        std::unique_ptr<StagingBuffer> pStagingBuffer = std::unique_ptr<StagingBuffer>(Upload(data));
        //Init(pStagingBuffer.get());

		// Transition 0: Layout: undefined->transfer, Queue: transfer
		VkImageLayout newLayout0 = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		VkPipelineStageFlags2 srcStage0 = pipelineStage::topOfPipe;
		VkPipelineStageFlags2 dstStage0 = pipelineStage::transfer;
		VkAccessFlags2 srcAccessMask0 = accessMask::topOfPipe::none;
		VkAccessFlags2 dstAccessMask0 = accessMask::transfer::transferWrite;
		m_pImage->TransitionLayout(newLayout0, srcStage0, dstStage0, srcAccessMask0, dstAccessMask0);

		// Upload: pStagingBuffer -> image
		pStagingBuffer->UploadToImage(Context::logicalDevice.GetTransferQueue(), m_pImage.get(), m_pImage->GetSubresourceRange().layerCount);

		// Transition 1: Layout: transfer->general, Queue: transfer->compute
		VkImageLayout newLayout1 = VK_IMAGE_LAYOUT_GENERAL;
		VkPipelineStageFlags2 srcStage1 = pipelineStage::transfer;
		VkPipelineStageFlags2 dstStage1 = pipelineStage::computeShader;
		VkAccessFlags2 srcAccessMask1 = accessMask::transfer::transferWrite;
		VkAccessFlags2 dstAccessMask1 = accessMask::computeShader::memoryRead;
		m_pImage->TransitionLayout(newLayout1, srcStage1, dstStage1, srcAccessMask1, dstAccessMask1);
	}
    StorageSampleTexture2d::StorageSampleTexture2d(const std::string& name, VkFormat format, const std::filesystem::path& path)
    {
        int channels = GetChannelCount(format);
        emberAssetLoader::Image imageAsset = emberAssetLoader::LoadImage(path, channels);

		m_name = name;
		m_width = imageAsset.width;
		m_height = imageAsset.height;
        m_channels = channels;
		m_format = format;
		m_descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        
		//std::unique_ptr<StagingBuffer> pStagingBuffer = std::unique_ptr<StagingBuffer>(Upload((void*)imageAsset.pixels.data()));
		//Init(pStagingBuffer.get());
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
	void StorageSampleTexture2d::Init(StagingBuffer* pStagingBuffer)
    {

    }
	StagingBuffer* StorageSampleTexture2d::Upload(void* data)
	{
		// Copy: pixelData -> pStagingBuffer
		uint64_t bufferSize = 4 * m_channels * m_width * m_height * BytesPerChannel(m_format);
		StagingBuffer* pStagingBuffer = new StagingBuffer(bufferSize, m_name);
		pStagingBuffer->SetData(data, bufferSize);

		// Define subresource range:
		VkImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = 1;
		subresourceRange.layerCount = 1;

		// Create image:
		VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		VkImageCreateFlags imageFlags = 0;
		VkMemoryPropertyFlags memoryFlags = 0;
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
		DeviceQueue queue = Context::logicalDevice.GetTransferQueue();
		CreateImage(subresourceRange, m_format, usageFlags, imageFlags, memoryFlags, viewType, queue);
		NAME_VK_IMAGE(m_pImage->GetVkImage(), "StorageSampleTexture2d " + m_name);

        return pStagingBuffer;
	}
}