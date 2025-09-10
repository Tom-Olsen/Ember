#include "sampleTexture2d.h"
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
#include <memory>



namespace vulkanRendererBackend
{
	// Constructor/Desctructor:
	SampleTexture2d::SampleTexture2d(const std::string& name, float value)
	{
		std::unique_ptr<StagingBuffer> pStagingBuffer = std::unique_ptr<StagingBuffer>(Load(name, value));
		Init(pStagingBuffer.get());
	}
	SampleTexture2d::SampleTexture2d(const std::string& name, iMath::Float4 color)
	{
		std::unique_ptr<StagingBuffer> pStagingBuffer = std::unique_ptr<StagingBuffer>(Load(name, color));
		Init(pStagingBuffer.get());
	}
	SampleTexture2d::SampleTexture2d(const std::string& name, VkFormat format, const std::filesystem::path& path)
	{
		std::unique_ptr<StagingBuffer> pStagingBuffer = std::unique_ptr<StagingBuffer>(Load(name, format, path));
		Init(pStagingBuffer.get());
	}
	SampleTexture2d::SampleTexture2d(const std::string& name, VkFormat format, const std::filesystem::path& path, TextureBatchUploader& batchUploader)
	{
		StagingBuffer* pStagingBuffer = Load(name, format, path);
		batchUploader.EnqueueTexture(pStagingBuffer, this);
	}
	SampleTexture2d::~SampleTexture2d()
	{

	}



	// Public methods:
	void SampleTexture2d::RecordGpuCommands(VkCommandBuffer& transferCommandBuffer, VkCommandBuffer& graphicsCommandBuffer, StagingBuffer* pStagingBuffer)
	{
		// Transition: Layout: undefined->dstTransfer, Queue: transfer
		{
			VkImageLayout newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			VkPipelineStageFlags2 srcStage = pipelineStage::topOfPipe;
			VkPipelineStageFlags2 dstStage = pipelineStage::transfer;
			VkAccessFlags2 srcAccessMask = accessMask::transfer::none;
			VkAccessFlags2 dstAccessMask = accessMask::transfer::transferWrite;
			m_pImage->TransitionLayout(transferCommandBuffer, newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
		}

		// Upload: pStagingBuffer -> image
		pStagingBuffer->UploadToImage(transferCommandBuffer, m_pImage.get(), m_pImage->GetSubresourceRange().layerCount);

		// Final transition, layout: transfer->shaderRead
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



	// Private methods:
	void SampleTexture2d::Init(StagingBuffer* pStagingBuffer)
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
	StagingBuffer* SampleTexture2d::Load(const std::string& name, VkFormat format, const std::filesystem::path& path)
	{
		m_name = name;
		if (format == VK_FORMAT_R8_UNORM)
			m_channels = 1; // Grayscale
		else if (format == VK_FORMAT_R8G8B8A8_UNORM || format == VK_FORMAT_R8G8B8A8_SRGB)
			m_channels = 4; // RGBA
		else
			throw std::runtime_error("Unsupported texture format for SampleTexture2d!");
		m_descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

		// Load pixelData:
		stbi_set_flip_vertically_on_load(true);
		stbi_uc* pPixels = stbi_load(path.string().c_str(), &m_width, &m_height, nullptr, m_channels);
		if (!pPixels)
			throw std::runtime_error("Failed to load texture image!");

		// Upload: pixelData -> pStagingBuffer
		uint64_t bufferSize = m_channels * m_width * m_height * BytesPerChannel(format);
		StagingBuffer* pStagingBuffer = new StagingBuffer(bufferSize, m_name);
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
		NAME_VK_IMAGE(m_pImage->GetVkImage(), "SampleTexture2d " + m_name);

		return pStagingBuffer;
	}
	StagingBuffer* SampleTexture2d::Load(const std::string& name, float value)
	{
		m_name = name;
		m_channels = 1;
		m_descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		m_width = 1;
		m_height = 1;
		VkFormat format = VK_FORMAT_R8_UNORM;

		// Clamp and convert:
		uint8_t pixel = static_cast<uint8_t>(std::clamp(value, 0.0f, 1.0f) * 255.0f);

		// Upload: pixel -> pStagingBuffer
		uint64_t bufferSize = m_channels * m_width * m_height * BytesPerChannel(format);
		StagingBuffer* pStagingBuffer = new StagingBuffer(bufferSize, m_name);
		pStagingBuffer->SetData(&pixel, bufferSize);

		// Define subresource range:
		VkImageSubresourceRange subresourceRange{};
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.layerCount = 1;
		subresourceRange.levelCount = 1; // only 1 mip level for 1x1 pixels.

		// Create image:
		VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		VkImageCreateFlags imageFlags = 0;
		VkMemoryPropertyFlags memoryFlags = 0;
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
		DeviceQueue queue = Context::logicalDevice.GetTransferQueue();
		CreateImage(subresourceRange, format, usageFlags, imageFlags, memoryFlags, viewType, queue);
		NAME_VK_IMAGE(m_pImage->GetVkImage(), "SampleTexture2d " + m_name);

		return pStagingBuffer;
	}
	StagingBuffer* SampleTexture2d::Load(const std::string& name, const iMath::Float4& color)
	{
		m_name = name;
		m_channels = 4;
		m_descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		m_width = 1;
		m_height = 1;
		VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;

		// Clamp and convert:
		uint8_t pixel[4];
		pixel[0] = static_cast<uint8_t>(std::clamp(color[0], 0.0f, 1.0f) * 255.0f);
		pixel[1] = static_cast<uint8_t>(std::clamp(color[1], 0.0f, 1.0f) * 255.0f);
		pixel[2] = static_cast<uint8_t>(std::clamp(color[2], 0.0f, 1.0f) * 255.0f);
		pixel[3] = static_cast<uint8_t>(std::clamp(color[3], 0.0f, 1.0f) * 255.0f);

		// Upload: pixel -> pStagingBuffer
		uint64_t bufferSize = m_channels * m_width * m_height * BytesPerChannel(format);
		StagingBuffer* pStagingBuffer = new StagingBuffer(bufferSize, m_name);
		pStagingBuffer->SetData(pixel, bufferSize);

		// Define subresource range:
		VkImageSubresourceRange subresourceRange{};
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.layerCount = 1;
		subresourceRange.levelCount = 1; // only 1 mip level for 1x1 pixels.

		// Create image:
		VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		VkImageCreateFlags imageFlags = 0;
		VkMemoryPropertyFlags memoryFlags = 0;
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
		DeviceQueue queue = Context::logicalDevice.GetTransferQueue();
		CreateImage(subresourceRange, format, usageFlags, imageFlags, memoryFlags, viewType, queue);
		NAME_VK_IMAGE(m_pImage->GetVkImage(), "SampleTexture2d " + m_name);

		return pStagingBuffer;
	}
}