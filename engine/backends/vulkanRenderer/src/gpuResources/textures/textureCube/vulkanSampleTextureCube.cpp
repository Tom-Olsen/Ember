#include "vulkanSampleTextureCube.h"
#include "assetLoader.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanAccessMasks.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include "vulkanPipelineStages.h"
#include "vulkanSingleTimeCommand.h"
#include "vulkanStagingBuffer.h"
#include "vulkanTextureBatchUploader.h"
#include <array>
#include <cstring>
#include <memory>



namespace vulkanRendererBackend
{
	// Constructor/Destructor:
	SampleTextureCube::SampleTextureCube(const std::string& name, VkFormat format, int width, int height, const std::array<void*, 6>& data)
	{
		if (!IsValidImageFormat(format))
			throw std::runtime_error("SampleTextureCube '" + name + "' uses unsuported format: " + std::to_string(static_cast<int>(format)));

		m_name = name;
		m_width = width;
		m_height = height;
		m_channels = GetChannelCount(format);
		m_format = format;
		m_descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

		std::unique_ptr<StagingBuffer> pStagingBuffer = std::unique_ptr<StagingBuffer>(Upload(data));
		Init(pStagingBuffer.get());

		NAME_VK_IMAGE(m_pImage->GetVkImage(), "SampleTextureCube " + m_name);
	}
	SampleTextureCube::SampleTextureCube(const std::string& name, VkFormat format, const std::filesystem::path& path)
    {
		if (!IsValidImageFormat(format))
			throw std::runtime_error("SampleTextureCube '" + name + "' uses unsuported format: " + std::to_string(static_cast<int>(format)));

		m_name = name;
		m_format = format;
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

        // Load all 6 images:
        m_channels = GetChannelCount(format);
        std::array<void*, 6> faces;
		for (uint32_t i = 0; i < 6; i++)
        {
            emberAssetLoader::Image imageAsset = emberAssetLoader::LoadImageFile(filePaths[i], m_channels);
            faces[i] = (void*)imageAsset.pixels.data();
		    m_width = imageAsset.width;
		    m_height = imageAsset.height;
        }

        // Upload data:
		std::unique_ptr<StagingBuffer> pStagingBuffer = std::unique_ptr<StagingBuffer>(Upload(faces));
		Init(pStagingBuffer.get());

		NAME_VK_IMAGE(m_pImage->GetVkImage(), "SampleTextureCube " + m_name);
    }
	SampleTextureCube::~SampleTextureCube()
	{

	}



	// Public methods:
	void SampleTextureCube::RecordGpuCommands(VkCommandBuffer& transferCommandBuffer, VkCommandBuffer& graphicsCommandBuffer, StagingBuffer* pStagingBuffer)
	{
		// Transition0: Layout: undefined->transfer, Queue: transfer
		{
			VkImageLayout newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			VkPipelineStageFlags2 srcStage = pipelineStage::topOfPipe;
			VkPipelineStageFlags2 dstStage = pipelineStage::transfer;
			VkAccessFlags2 srcAccessMask = accessMask::topOfPipe::none;
			VkAccessFlags2 dstAccessMask = accessMask::transfer::transferWrite;
			m_pImage->TransitionLayout(transferCommandBuffer, newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
		}

		// Upload: pStagingBuffer -> texture
		pStagingBuffer->UploadToTexture(transferCommandBuffer, this, m_pImage->GetSubresourceRange().layerCount);

		// Transition1: Layout: transfer->shaderRead
		{
			VkImageLayout newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			VkPipelineStageFlags2 srcStage = pipelineStage::transfer;
			VkPipelineStageFlags2 dstStage = pipelineStage::fragmentShader;
			VkAccessFlags2 srcAccessMask = accessMask::transfer::transferWrite;
			VkAccessFlags2 dstAccessMask = accessMask::fragmentShader::shaderRead;
			m_pImage->TransitionLayout(transferCommandBuffer, newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
		}
	}



	// Private methods:
	StagingBuffer* SampleTextureCube::Upload(const std::array<void*, 6>& data)
	{
		// Write in continuous data:
		uint64_t faceSize = m_channels * m_width * m_height * BytesPerChannel(m_format);
		uint64_t bufferSize = 6 * faceSize;
		uint8_t* pFacePixels = new uint8_t[bufferSize];
		for (int i = 0; i < 6; i++)
			std::memcpy(pFacePixels + i * faceSize, data[i], faceSize);

		// Upload: data -> pStagingBuffer
		StagingBuffer* pStagingBuffer = new StagingBuffer(bufferSize, m_name);
		pStagingBuffer->SetData(pFacePixels, bufferSize);
		delete[] pFacePixels;

		// Define subresource range:
		VkImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = 1;	// mipmapping makes no sense for skyboxes.
		subresourceRange.layerCount = 6;

		// Create image:
		VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		VkImageCreateFlags imageFlags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_CUBE;
		DeviceQueue queue = Context::logicalDevice.GetTransferQueue();
		CreateImage(subresourceRange, m_format, usageFlags, imageFlags, memoryFlags, viewType, queue);

		return pStagingBuffer;
	}
	void SampleTextureCube::Init(StagingBuffer* pStagingBuffer)
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
		delete pStagingBuffer;
	}
}