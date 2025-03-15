#include "storageSampleTexture2d.h"
#include "logger.h"
#include "stb_image.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanContext.h"



namespace emberEngine
{
	// Constructor/Desctructor:
	StorageSampleTexture2d::StorageSampleTexture2d(const std::string& name, VkFormat format, int width, int height)
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

		// Create staging buffer:
		uint64_t bufferSize = 4 * m_channels * m_width * m_height * BytesPerChannel(format);
		VmaBuffer stagingBuffer = VmaBuffer::StagingBuffer(bufferSize, pPixels);

		// Define subresource range:
		VkImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = 1;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.layerCount = 1;

		// Create image:
		VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		VkImageCreateFlags imageFlags = 0;
		VkMemoryPropertyFlags memoryFlags = 0;
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
		VulkanQueue queue = VulkanContext::pLogicalDevice->GetTransferQueue();
		m_pImage = std::unique_ptr<VmaImage>(CreateImage(subresourceRange, format, usageFlags, imageFlags, memoryFlags, viewType, queue));


		// Transition 0: Layout: undefined->transfer, Queue: transfer
		VkImageLayout newLayout0 = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		VkPipelineStageFlags2 srcStage0 = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
		VkPipelineStageFlags2 dstStage0 = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
		VkAccessFlags2 srcAccessMask0 = VK_ACCESS_2_NONE;
		VkAccessFlags2 dstAccessMask0 = VK_ACCESS_2_TRANSFER_WRITE_BIT;
		m_pImage->TransitionLayout(newLayout0, srcStage0, dstStage0, srcAccessMask0, dstAccessMask0);

		VmaBuffer::CopyBufferToImage(&stagingBuffer, m_pImage.get(), VulkanContext::pLogicalDevice->GetTransferQueue(), subresourceRange.layerCount);

		// Transition 1: Layout: transfer->general, Queue: transfer->compute
		VkImageLayout newLayout1 = VK_IMAGE_LAYOUT_GENERAL;
		VulkanQueue newQueue1 = VulkanContext::pLogicalDevice->GetComputeQueue();
		VkPipelineStageFlags2 srcStage1 = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
		VkPipelineStageFlags2 dstStage1 = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
		VkAccessFlags2 srcAccessMask1 = VK_ACCESS_2_TRANSFER_WRITE_BIT;
		VkAccessFlags2 dstAccessMask1 = VK_ACCESS_2_MEMORY_READ_BIT;
		m_pImage->TransitionLayoutAndQueue(newLayout1, newQueue1, srcStage1, dstStage1, srcAccessMask1, dstAccessMask1);

		// Free memory:
		delete[] pPixels;
	}
	StorageSampleTexture2d::StorageSampleTexture2d(const std::string& name, VkFormat format, const std::filesystem::path& filePath)
	{
		m_name = name;
		m_channels = STBI_rgb_alpha;	// 4 8-bit channels

		// Load image:
		stbi_set_flip_vertically_on_load(true);
		stbi_uc* pPixels = stbi_load(filePath.string().c_str(), &m_width, &m_height, nullptr, m_channels);
		if (!pPixels)
			throw std::runtime_error("Failed to load texture image!");

		// Create staging buffer:
		uint64_t bufferSize = 4 * m_width * m_height;
		VmaBuffer stagingBuffer = VmaBuffer::StagingBuffer(bufferSize, pPixels);

		// Define subresource range:
		VkImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = 1;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.layerCount = 1;

		// Create image:
		VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		VkImageCreateFlags imageFlags = 0;
		VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
		VulkanQueue queue = VulkanContext::pLogicalDevice->GetTransferQueue();
		m_pImage = std::unique_ptr<VmaImage>(CreateImage(subresourceRange, format, usageFlags, imageFlags, memoryFlags, viewType, queue));

		// Transition 0: Layout: undefined->transfer, Queue: transfer
		VkImageLayout newLayout0 = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		VkPipelineStageFlags2 srcStage0 = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
		VkPipelineStageFlags2 dstStage0 = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
		VkAccessFlags2 srcAccessMask0 = VK_ACCESS_2_NONE;
		VkAccessFlags2 dstAccessMask0 = VK_ACCESS_2_TRANSFER_WRITE_BIT;
		m_pImage->TransitionLayout(newLayout0, srcStage0, dstStage0, srcAccessMask0, dstAccessMask0);

		VmaBuffer::CopyBufferToImage(&stagingBuffer, m_pImage.get(), VulkanContext::pLogicalDevice->GetTransferQueue(), subresourceRange.layerCount);

		// Transition 1: Layout: transfer->general, Queue: transfer->compute
		VkImageLayout newLayout1 = VK_IMAGE_LAYOUT_GENERAL;
		VulkanQueue newQueue1 = VulkanContext::pLogicalDevice->GetComputeQueue();
		VkPipelineStageFlags2 srcStage1 = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
		VkPipelineStageFlags2 dstStage1 = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
		VkAccessFlags2 srcAccessMask1 = VK_ACCESS_2_TRANSFER_WRITE_BIT;
		VkAccessFlags2 dstAccessMask1 = VK_ACCESS_2_MEMORY_READ_BIT;
		m_pImage->TransitionLayoutAndQueue(newLayout1, newQueue1, srcStage1, dstStage1, srcAccessMask1, dstAccessMask1);

		// Free memory:
		stbi_image_free(pPixels);
	}
	StorageSampleTexture2d::~StorageSampleTexture2d()
	{

	}
}