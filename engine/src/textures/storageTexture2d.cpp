#include "storageTexture2d.h"
#include "logger.h"
#include "stagingBuffer.h"
#include "stb_image.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanAccessMasks.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include "vulkanPipelineStages.h"



namespace emberEngine
{
	using namespace vulkanBackend;



	// Constructor/Desctructor:
	StorageTexture2d::StorageTexture2d(const std::string& name, VkFormat format, int width, int height)
	{
		m_name = name;
		m_width = width;
		m_height = height;
		m_channels = STBI_rgb_alpha;	// 4 channels
		m_descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

		// Create zero initialized pixels:
		stbi_uc* pPixels = new stbi_uc[m_width * m_height * m_channels];
		for (uint32_t i = 0; i < m_width * m_height * m_channels; i++)
			pPixels[i] = 0;

		// Define subresource range:
		VkImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = 1;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.layerCount = 1;

		// Create image:
		VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
		VkImageCreateFlags imageFlags = 0;
		VkMemoryPropertyFlags memoryFlags = 0;
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
		DeviceQueue queue = Context::logicalDevice.GetTransferQueue();
		CreateImage(subresourceRange, format, usageFlags, imageFlags, memoryFlags, viewType, queue);
		NAME_VK_IMAGE(m_pImage->GetVkImage(), "StorageTexture2d " + m_name);

		// Transition 0: Layout: undefined->transfer, Queue: transfer
		VkImageLayout newLayout0 = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		VkPipelineStageFlags2 srcStage0 = pipelineStage::topOfPipe;
		VkPipelineStageFlags2 dstStage0 = pipelineStage::transfer;
		VkAccessFlags2 srcAccessMask0 = accessMask::topOfPipe::none;
		VkAccessFlags2 dstAccessMask0 = accessMask::transfer::transferWrite;
		m_pImage->TransitionLayout(newLayout0, srcStage0, dstStage0, srcAccessMask0, dstAccessMask0);

		// Copy: pixelData -> stagingBuffer -> image
		uint64_t bufferSize = 4 * m_channels * m_width * m_height * BytesPerChannel(format);
		StagingBuffer stagingBuffer(bufferSize, m_name);
		stagingBuffer.SetData(pPixels, bufferSize);
		stagingBuffer.UploadToImage(Context::logicalDevice.GetTransferQueue(), m_pImage.get(), subresourceRange.layerCount);

		// Transition 1: Layout: transfer->general, Queue: transfer->compute
		VkImageLayout newLayout1 = VK_IMAGE_LAYOUT_GENERAL;
		VkPipelineStageFlags2 srcStage1 = pipelineStage::transfer;
		VkPipelineStageFlags2 dstStage1 = pipelineStage::computeShader;
		VkAccessFlags2 srcAccessMask1 = accessMask::transfer::transferWrite;
		VkAccessFlags2 dstAccessMask1 = accessMask::computeShader::memoryRead | accessMask::computeShader::memoryWrite;
		m_pImage->TransitionLayout(newLayout1, srcStage1, dstStage1, srcAccessMask1, dstAccessMask1);

		// Free memory:
		delete[] pPixels;
	}
	StorageTexture2d::StorageTexture2d(const std::string& name, VkFormat format, const std::filesystem::path& filePath)
	{
		m_name = name;
		m_channels = STBI_rgb_alpha;	// 4 channels

		// Load image:
		stbi_set_flip_vertically_on_load(true);
		stbi_uc* pPixels = stbi_load(filePath.string().c_str(), &m_width, &m_height, nullptr, m_channels);
		if (!pPixels)
			throw std::runtime_error("Failed to load texture image!");

		// Define subresource range:
		VkImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = 1;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.layerCount = 1;

		// Create image:
		VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
		VkImageCreateFlags imageFlags = 0;
		VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
		DeviceQueue queue = Context::logicalDevice.GetTransferQueue();
		CreateImage(subresourceRange, format, usageFlags, imageFlags, memoryFlags, viewType, queue);

		// Transition 0: Layout: undefined->transfer, Queue: transfer
		VkImageLayout newLayout0 = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		VkPipelineStageFlags2 srcStage0 = pipelineStage::topOfPipe;
		VkPipelineStageFlags2 dstStage0 = pipelineStage::transfer;
		VkAccessFlags2 srcAccessMask0 = accessMask::topOfPipe::none;
		VkAccessFlags2 dstAccessMask0 = accessMask::transfer::transferWrite;
		m_pImage->TransitionLayout(newLayout0, srcStage0, dstStage0, srcAccessMask0, dstAccessMask0);

		// Copy: pixelData -> stagingBuffer -> image
		uint64_t bufferSize = 4 * m_width * m_height;
		StagingBuffer stagingBuffer(bufferSize, m_name);
		stagingBuffer.SetData(pPixels, bufferSize);
		stagingBuffer.UploadToImage(Context::logicalDevice.GetTransferQueue(), m_pImage.get(), subresourceRange.layerCount);

		// Transition 1: Layout: transfer->general, Queue: transfer->compute
		VkImageLayout newLayout1 = VK_IMAGE_LAYOUT_GENERAL;
		VkPipelineStageFlags2 srcStage1 = pipelineStage::transfer;
		VkPipelineStageFlags2 dstStage1 = pipelineStage::computeShader;
		VkAccessFlags2 srcAccessMask1 = accessMask::transfer::transferWrite;
		VkAccessFlags2 dstAccessMask1 = accessMask::computeShader::memoryRead;
		m_pImage->TransitionLayout(newLayout1, srcStage1, dstStage1, srcAccessMask1, dstAccessMask1);

		// Free memory:
		stbi_image_free(pPixels);
	}
	StorageTexture2d::~StorageTexture2d()
	{

	}



	// Pritvat methods:
	StagingBuffer* StorageTexture2d::Init(const std::string& name, VkFormat format, const std::filesystem::path& path)
	{
		LOG_WARN("StorageTexture2d::Init(...) not implemented yet.");
		return nullptr;
	}



	// Public methods:
	void StorageTexture2d::RecordGpuCommands(VkCommandBuffer& transferCommandBuffer, VkCommandBuffer& graphicsCommandBuffer, StagingBuffer* pStagingBuffer)
	{
		LOG_WARN("StorageTexture2d::RecordGpuCommands(...) not implemented yet.");
	}
}