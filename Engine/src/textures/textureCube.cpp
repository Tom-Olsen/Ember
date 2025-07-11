#include "textureCube.h"
#include "logger.h"
#include "stagingBuffer.h"
#include "stb_image.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanAccessMasks.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include "vulkanPipelineStages.h"
#include "vulkanSingleTimeCommand.h"
#include "vulkanTextureBatchUploader.h"
#include <array>



namespace emberEngine
{
	using namespace vulkanBackend;



	// Constructor/Destructor:
	TextureCube::TextureCube(const std::string& name, VkFormat format, const std::filesystem::path& path)
	{
		StagingBuffer* pStagingBuffer = Init(name, format, path);

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
	TextureCube::TextureCube(const std::string& name, VkFormat format, const std::filesystem::path& path, vulkanBackend::TextureBatchUploader& batchUploader)
	{
		StagingBuffer* pStagingBuffer = Init(name, format, path);
		batchUploader.EnqueueTexture(pStagingBuffer, this);
	}
	TextureCube::~TextureCube()
	{

	}



	// Pritvat methods:
	StagingBuffer* TextureCube::Init(const std::string& name, VkFormat format, const std::filesystem::path& path)
	{
		m_name = name;
		m_channels = STBI_rgb_alpha;	// 4 8-bit channels
		m_descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

		// Check if folder exists:
		if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
			throw std::runtime_error("Given path '" + path.string() + "' does not exist");

		// Get paths to the 6 images:
		const std::array<std::string, 6> directions = { "px", "nx", "py", "ny", "pz", "nz" };
		std::array<std::filesystem::path, 6> filePaths;
		for (uint32_t i = 0; i < 6; i++)
			for (const auto& entry : std::filesystem::directory_iterator(path))
				if (entry.is_regular_file() && entry.path().stem() == directions[i])
					filePaths[i] = entry.path();

		// Get image info (width and height):
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

		// Upload: pixelData -> pStagingBuffer
		StagingBuffer* pStagingBuffer = new StagingBuffer(bufferSize);
		pStagingBuffer->SetData(pFacePixels, bufferSize);
		delete[] pFacePixels;

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
		DeviceQueue queue = Context::logicalDevice.GetTransferQueue();
		CreateImage(subresourceRange, format, usageFlags, imageFlags, memoryFlags, viewType, queue);
		NAME_VK_IMAGE(m_pImage->GetVkImage(), "textureCube2d " + name);

		return pStagingBuffer;
	}



	// Public methods:
	void TextureCube::RecordGpuCommands(VkCommandBuffer& transferCommandBuffer, VkCommandBuffer& graphicsCommandBuffer, StagingBuffer* pStagingBuffer)
	{
		// Transition0: Layout: undefined->transfer, Queue: transfer
		VkImageLayout newLayout0 = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		VkPipelineStageFlags2 srcStage0 = pipelineStage::topOfPipe;
		VkPipelineStageFlags2 dstStage0 = pipelineStage::transfer;
		VkAccessFlags2 srcAccessMask0 = accessMask::topOfPipe::none;
		VkAccessFlags2 dstAccessMask0 = accessMask::transfer::transferWrite;
		m_pImage->TransitionLayout(transferCommandBuffer, newLayout0, srcStage0, dstStage0, srcAccessMask0, dstAccessMask0);

		// Upload: pStagingBuffer -> image
		pStagingBuffer->UploadToImage(transferCommandBuffer, m_pImage.get(), m_pImage->GetSubresourceRange().layerCount);

		// Transition1: Layout: transfer->shader read
		VkImageLayout newLayout1 = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		VkPipelineStageFlags2 srcStage1 = pipelineStage::transfer;
		VkPipelineStageFlags2 dstStage1 = pipelineStage::fragmentShader;
		VkAccessFlags2 srcAccessMask1 = accessMask::transfer::transferWrite;
		VkAccessFlags2 dstAccessMask1 = accessMask::fragmentShader::shaderRead;
		m_pImage->TransitionLayout(transferCommandBuffer, newLayout1, srcStage1, dstStage1, srcAccessMask1, dstAccessMask1);
	}
}