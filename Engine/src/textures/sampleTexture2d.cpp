#include "sampleTexture2d.h"
#include "logger.h"
#include "stagingBuffer.h"
#include "stb_image.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanContext.h"



namespace emberEngine
{
	using namespace vulkanBackend;



	// Constructor/Desctructor:
	SampleTexture2d::SampleTexture2d(const std::string& name, VkFormat format, const std::filesystem::path& filePath)
	{
		m_name = name;
		m_channels = STBI_rgb_alpha;	// 4 channels
		m_descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

		// Load image:
		stbi_set_flip_vertically_on_load(true);
		stbi_uc* pPixels = stbi_load(filePath.string().c_str(), &m_width, &m_height, nullptr, m_channels);
		if (!pPixels)
			throw std::runtime_error("Failed to load texture image!");

		// Define subresource range:
		VkImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = static_cast<uint32_t>(std::floor(std::log2(std::max(m_width, m_height)))) + 1;	// mip levels
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.layerCount = 1;

		// Create image:
		VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		VkImageCreateFlags imageFlags = 0;
		VkMemoryPropertyFlags memoryFlags = 0;
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
		DeviceQueue queue = Context::logicalDevice.GetTransferQueue();
		m_pImage = std::unique_ptr<VmaImage>(CreateImage(subresourceRange, format, usageFlags, imageFlags, memoryFlags, viewType, queue));

		// Transition 0: Layout: undefined->transfer, Queue: transfer
		VkImageLayout newLayout0 = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		VkPipelineStageFlags2 srcStage0 = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
		VkPipelineStageFlags2 dstStage0 = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
		VkAccessFlags2 srcAccessMask0 = VK_ACCESS_2_NONE;
		VkAccessFlags2 dstAccessMask0 = VK_ACCESS_2_TRANSFER_WRITE_BIT;
		m_pImage->TransitionLayout(newLayout0, srcStage0, dstStage0, srcAccessMask0, dstAccessMask0);

		// Copy: pixelData -> stagingBuffer -> image
		uint64_t bufferSize = m_channels * m_width * m_height * BytesPerChannel(format);
		StagingBuffer stagingBuffer(bufferSize);
		stagingBuffer.SetData(pPixels, bufferSize);
		stagingBuffer.UploadToImage(m_pImage.get(), Context::logicalDevice.GetTransferQueue(), subresourceRange.layerCount);

		// Transition 1: Layout: transfer, Queue: transfer->graphics
		DeviceQueue newQueue1 = Context::logicalDevice.GetGraphicsQueue();
		VkPipelineStageFlags2 srcStage1 = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
		VkPipelineStageFlags2 dstStage1 = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
		VkAccessFlags2 srcAccessMask1 = VK_ACCESS_2_TRANSFER_WRITE_BIT;
		VkAccessFlags2 dstAccessMask1 = VK_ACCESS_2_TRANSFER_WRITE_BIT;
		m_pImage->TransitionQueue(newQueue1, srcStage1, dstStage1, srcAccessMask1, dstAccessMask1);

		// Mipmapping with final transition, layout: transfer->shader read
		m_pImage->GenerateMipmaps(subresourceRange.levelCount);

		// Free memory:
		stbi_image_free(pPixels);
	}
	SampleTexture2d::~SampleTexture2d()
	{

	}
}