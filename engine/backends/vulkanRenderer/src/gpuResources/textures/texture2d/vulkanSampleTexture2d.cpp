#include "vulkanSampleTexture2d.h"
#include "assetLoader.h"
#include "emberMath.h"
#include "vmaImage.h"
#include "vulkanAccessMasks.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include "vulkanPipelineStages.h"
#include "vulkanSingleTimeCommand.h"
#include "vulkanStagingBuffer.h"
#include <memory>



namespace vulkanRendererBackend
{
	// Constructor/Desctructor:
	SampleTexture2d::SampleTexture2d(const std::string& name, VkFormat format, int width, int height, void* data)
	{
		if (!IsValidImageFormat(format))
			throw std::runtime_error("SampleTexture2d '" + name + "' uses unsuported format: " + std::to_string(static_cast<int>(format)));

        m_name = name;
		m_width = width;
		m_height = height;
        m_channels = GetChannelCount(format);
		m_format = format;
		m_descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

        std::unique_ptr<StagingBuffer> pStagingBuffer = std::unique_ptr<StagingBuffer>(Staging(data));
        Upload(pStagingBuffer.get());

		NAME_VK_IMAGE(m_pImage->GetVkImage(), "SampleTexture2d " + m_name);
	}
	SampleTexture2d::SampleTexture2d(const std::string& name, VkFormat format, const std::filesystem::path& path)
	{
		if (!IsValidImageFormat(format))
			throw std::runtime_error("SampleTexture2d '" + name + "' uses unsuported format: " + std::to_string(static_cast<int>(format)));

        int channels = GetChannelCount(format);
        emberAssetLoader::Image imageAsset = emberAssetLoader::LoadImageFile(path, channels);

		m_name = name;
		m_width = imageAsset.width;
		m_height = imageAsset.height;
        m_channels = channels;
		m_format = format;
		m_descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        
		std::unique_ptr<StagingBuffer> pStagingBuffer = std::unique_ptr<StagingBuffer>(Staging((void*)imageAsset.pixels.data()));
		Upload(pStagingBuffer.get());

		NAME_VK_IMAGE(m_pImage->GetVkImage(), "SampleTexture2d " + m_name);
	}
	SampleTexture2d::~SampleTexture2d()
	{

	}



	// Private methods:
	StagingBuffer* SampleTexture2d::Staging(void* data)
    {
		// Upload: data -> pStagingBuffer
		uint64_t bufferSize = m_channels * m_width * m_height * BytesPerChannel(m_format);
		StagingBuffer* pStagingBuffer = new StagingBuffer(bufferSize, m_name);
		pStagingBuffer->SetData(data, bufferSize);

		// Define subresource range:
		VkImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.layerCount = 1;
		subresourceRange.levelCount = static_cast<uint32_t>(math::Floor(math::Log2(math::Max(m_width, m_height)))) + 1;	// mip levels

		// Create image:
		VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		VkImageCreateFlags imageFlags = 0;
		VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
		DeviceQueue queue = Context::logicalDevice.GetTransferQueue();
		CreateImage(subresourceRange, m_format, usageFlags, imageFlags, memoryFlags, viewType, queue);

		return pStagingBuffer;
    }
	void SampleTexture2d::Upload(StagingBuffer* pStagingBuffer)
	{
		const DeviceQueue& transferQueue = Context::logicalDevice.GetTransferQueue();
		const DeviceQueue& graphicsQueue = Context::logicalDevice.GetGraphicsQueue();
		if (transferQueue.queue != graphicsQueue.queue)
		{
			VkCommandBuffer transferCommandBuffer = SingleTimeCommand::BeginCommand(transferQueue);
			VkCommandBuffer graphicsCommandBuffer = SingleTimeCommand::BeginCommand(graphicsQueue);
			RecordGpuCommands(transferCommandBuffer, graphicsCommandBuffer, pStagingBuffer);
			SingleTimeCommand::EndLinkedCommands(transferQueue, graphicsQueue, pipelineStage::transfer);
		}
		else
		{
			VkCommandBuffer commandBuffer = SingleTimeCommand::BeginCommand(graphicsQueue);
			RecordGpuCommands(commandBuffer, commandBuffer, pStagingBuffer);
			SingleTimeCommand::EndCommand(graphicsQueue);
		}
	}
	void SampleTexture2d::RecordGpuCommands(VkCommandBuffer& transferCommandBuffer, VkCommandBuffer& graphicsCommandBuffer, StagingBuffer* pStagingBuffer)
	{
		// Transition 0: Layout: undefined->dstTransfer, Queue: transfer
		{
			VkImageLayout newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			VkPipelineStageFlags2 srcStage = pipelineStage::topOfPipe;
			VkPipelineStageFlags2 dstStage = pipelineStage::transfer;
			VkAccessFlags2 srcAccessMask = accessMask::transfer::none;
			VkAccessFlags2 dstAccessMask = accessMask::transfer::transferWrite;
			m_pImage->TransitionLayout(transferCommandBuffer, newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
		}

		// Upload: pStagingBuffer -> texture
		pStagingBuffer->UploadToTexture(transferCommandBuffer, this, m_pImage->GetSubresourceRange().layerCount);

		// Transition 1: Layout: transfer->shaderRead
		// With mipmapping: Queue: graphics
		if (m_pImage->GetSubresourceRange().levelCount > 1)
			m_pImage->GenerateMipmaps(graphicsCommandBuffer, m_pImage->GetSubresourceRange().levelCount);
		// Without mipmapping: Queue: transfer
		else
		{
			VkImageLayout newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			VkPipelineStageFlags2 srcStage = pipelineStage::transfer;
			VkPipelineStageFlags2 dstStage = pipelineStage::fragmentShader;
			VkAccessFlags2 srcAccessMask = accessMask::transfer::transferWrite;
			VkAccessFlags2 dstAccessMask = accessMask::fragmentShader::shaderRead;
			m_pImage->TransitionLayout(transferCommandBuffer, newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
		}
	}
}