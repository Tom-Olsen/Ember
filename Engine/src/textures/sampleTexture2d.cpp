#include "sampleTexture2d.h"
#include "logger.h"
#include "stagingBuffer.h"
#include "stb_image.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanAccessMasks.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include "vulkanObjectToString.h"
#include "vulkanPipelineStages.h"
#include "vulkanSingleTimeCommand.h"
#include "vulkanTextureBatchUploader.h"



namespace emberEngine
{
	using namespace vulkanBackend;



	// Constructor/Desctructor:
	SampleTexture2d::SampleTexture2d(const std::string& name, VkFormat format, const std::filesystem::path& filePath)
	{
		StagingBuffer* pStagingBuffer = Init(name, format, filePath);

		// GPU commands:
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
		delete pStagingBuffer;
	}
	SampleTexture2d::SampleTexture2d(const std::string& name, VkFormat format, const std::filesystem::path& filePath, TextureBatchUploader& batchUploader)
	{
		StagingBuffer* pStagingBuffer = Init(name, format, filePath);
		batchUploader.EnqueueTexture(pStagingBuffer, this);
	}
	SampleTexture2d::~SampleTexture2d()
	{

	}



	// Pritvat methods:
	StagingBuffer* SampleTexture2d::Init(const std::string& name, VkFormat format, const std::filesystem::path& filePath)
	{
		m_name = name;
		m_channels = STBI_rgb_alpha;	// 4 channels
		m_descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

		// Load pixelData:
		stbi_set_flip_vertically_on_load(true);
		stbi_uc* pPixels = stbi_load(filePath.string().c_str(), &m_width, &m_height, nullptr, m_channels);
		if (!pPixels)
			throw std::runtime_error("Failed to load texture image!");

		// Upload: pixelData -> stagingBuffer
		uint64_t bufferSize = m_channels * m_width * m_height * BytesPerChannel(format);
		StagingBuffer* pStagingBuffer = new StagingBuffer(bufferSize);
		pStagingBuffer->SetData(pPixels, bufferSize);
		stbi_image_free(pPixels);

		// Define subresource range:
		VkImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.layerCount = 1;
		subresourceRange.levelCount = static_cast<uint32_t>(std::floor(std::log2(std::max(m_width, m_height)))) + 1;	// mip levels

		// Create image:
		VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		VkImageCreateFlags imageFlags = 0;
		VkMemoryPropertyFlags memoryFlags = 0;
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
		DeviceQueue queue = Context::logicalDevice.GetTransferQueue();
		CreateImage(subresourceRange, format, usageFlags, imageFlags, memoryFlags, viewType, queue);
		NAME_VK_IMAGE(m_pImage->GetVkImage(), name + " SampleTexture2d");

		return pStagingBuffer;
	}



	// Public methods:
	void SampleTexture2d::RecordGpuCommands(VkCommandBuffer& transferCommandBuffer, VkCommandBuffer& graphicsCommandBuffer, StagingBuffer* pStagingBuffer)
	{
		// Transition: Layout: undefined->transfer, Queue: transfer
		VkImageLayout newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		VkPipelineStageFlags2 srcStage = pipelineStage::topOfPipe;
		VkPipelineStageFlags2 dstStage = pipelineStage::transfer;
		VkAccessFlags2 srcAccessMask = accessMask::transfer::none;
		VkAccessFlags2 dstAccessMask = accessMask::transfer::transferWrite;
		m_pImage->TransitionLayout(transferCommandBuffer, newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);

		// Upload: stagingBuffer -> image
		pStagingBuffer->UploadToImage(transferCommandBuffer, m_pImage.get(), m_pImage->GetSubresourceRange().layerCount);

		// Mipmapping with final transition, layout: transfer->shader read
		m_pImage->GenerateMipmaps(graphicsCommandBuffer, m_pImage->GetSubresourceRange().levelCount);
	}
}