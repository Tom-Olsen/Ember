#include "textureCube.h"
#include "logger.h"
#include "stagingBuffer.h"
#include "stb_image.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanContext.h"
#include <array>



namespace emberEngine
{
	// Constructor/Destructor:
	TextureCube::TextureCube(const std::filesystem::path& folderPath, const std::string& name, VkFormat format)
	{
		m_name = name;
		m_channels = STBI_rgb_alpha;	// 4 8-bit channels
		m_descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

		// Check if folder exists:
		if (!std::filesystem::exists(folderPath) || !std::filesystem::is_directory(folderPath))
			throw std::runtime_error("Given path '" + folderPath.string() + "' does not exist");

		// Get paths to the 6 images:
		const std::array<std::string, 6> directions = { "px", "nx", "py", "ny", "pz", "nz" };
		std::array<std::filesystem::path, 6> filePaths;
		for (uint32_t i = 0; i < 6; i++)
			for (const auto& entry : std::filesystem::directory_iterator(folderPath))
				if (entry.is_regular_file() && entry.path().stem() == directions[i])
					filePaths[i] = entry.path();

		// Get image info:
		if (!stbi_info(filePaths[0].string().c_str(), &m_width, &m_height, nullptr))
			throw std::runtime_error("Failed to get image info for: " + filePaths[0].string());

		// Load all 6 faces into continous memory:
		stbi_set_flip_vertically_on_load(false);
		uint64_t bufferSize = 6 * m_channels * m_width * m_height * BytesPerChannel(format);
		stbi_uc* pFacePixels = new stbi_uc[bufferSize];
		for (int i = 0; i < 6; i++)
		{
			int w, h;	// dummy variables.
			stbi_uc* pPixels = stbi_load(filePaths[i].string().c_str(), &w, &h, nullptr, m_channels);
			if (!pPixels)
			{
				delete[] pFacePixels;
				stbi_image_free(pPixels);
				throw std::runtime_error("Failed to load cube texture face: " + filePaths[i].string());
			}
			memcpy(&pFacePixels[i * m_channels * m_width * m_height], pPixels, m_channels * m_width * m_height);
			stbi_image_free(pPixels);
		}

		// Define subresource range:
		VkImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = 1;	// mipmapping makes no sense for skyboxes
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.layerCount = 6;

		// Create image:
		VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		VkImageCreateFlags imageFlags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_CUBE;
		VulkanQueue queue = VulkanContext::pLogicalDevice->GetTransferQueue();
		m_pImage = std::unique_ptr<VmaImage>(CreateImage(subresourceRange, format, usageFlags, imageFlags, memoryFlags, viewType, queue));

		// Transition0: Layout: undefined->transfer, Queue: transfer
		VkImageLayout newLayout0 = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		VkPipelineStageFlags2 srcStage0 = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
		VkPipelineStageFlags2 dstStage0 = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
		VkAccessFlags2 srcAccessMask0 = VK_ACCESS_2_NONE;
		VkAccessFlags2 dstAccessMask0 = VK_ACCESS_2_TRANSFER_WRITE_BIT;
		m_pImage->TransitionLayout(newLayout0, srcStage0, dstStage0, srcAccessMask0, dstAccessMask0);

		// Copy: pixelData -> stagingBuffer -> image
		StagingBuffer stagingBuffer(bufferSize);
		stagingBuffer.SetData(pFacePixels, bufferSize);
		stagingBuffer.UploadToImage(m_pImage.get(), VulkanContext::pLogicalDevice->GetTransferQueue(), subresourceRange.layerCount);

		// Transition1: Layout: transfer->shader read, Queue: transfer->graphics
		VkImageLayout newLayout1 = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		VulkanQueue newQueue1 = VulkanContext::pLogicalDevice->GetGraphicsQueue();
		VkPipelineStageFlags2 srcStage1 = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
		VkPipelineStageFlags2 dstStage1 = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
		VkAccessFlags2 srcAccessMask1 = VK_ACCESS_2_TRANSFER_WRITE_BIT;
		VkAccessFlags2 dstAccessMask1 = VK_ACCESS_2_SHADER_READ_BIT;
		m_pImage->TransitionLayoutAndQueue(newLayout1, newQueue1, srcStage1, dstStage1, srcAccessMask1, dstAccessMask1);

		delete[] pFacePixels;
	}
	TextureCube::~TextureCube()
	{

	}
}