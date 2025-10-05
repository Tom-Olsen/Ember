#include "vulkanSampleTexture2d.h"
#include "assetLoader.h"
#include "emberMath.h"
#include "vmaImage.h"
#include "vulkanAccessMask.h"
#include "vulkanContext.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include "vulkanPipelineStage.h"
#include "vulkanSingleTimeCommand.h"
#include "vulkanStagingBuffer.h"
#include <memory>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Desctructor:
	SampleTexture2d::SampleTexture2d(const std::string& name, Format format, int width, int height)
	{
		Init(name, format, width, height);
	}
	SampleTexture2d::SampleTexture2d(const std::string& name, Format format, int width, int height, void* data)
	{
		Init(name, format, width, height);
		SetData(data);
	}
	SampleTexture2d::SampleTexture2d(const std::string& name, Format format, const std::filesystem::path& path)
	{
		emberAssetLoader::Image imageAsset = emberAssetLoader::LoadImageFile(path, GetChannelCount(format));
		Init(name, format, imageAsset.width, imageAsset.height);
		SetData((void*)imageAsset.pixels.data());
	}
	SampleTexture2d::~SampleTexture2d()
	{

	}



	// Movable:
	SampleTexture2d::SampleTexture2d(SampleTexture2d&& other) noexcept = default;
	SampleTexture2d& SampleTexture2d::operator=(SampleTexture2d&& other) noexcept = default;



	void SampleTexture2d::SetData(void* data)
	{
		std::unique_ptr<StagingBuffer> pStagingBuffer = std::unique_ptr<StagingBuffer>(StageData(data));
		Upload(pStagingBuffer.get());
	}
	


	// Private methods:
	void SampleTexture2d::Init(const std::string& name, Format format, int width, int height)
	{
		if (!IsValidImageFormat(format))
			throw std::runtime_error("SampleTexture2d '" + name + "' uses unsuported format: " + std::to_string(static_cast<int>(format)));

		m_name = name;
		m_width = width;
		m_height = height;
		m_channels = GetChannelCount(format);
		m_format = format;
		m_descriptorType = DescriptorTypes::sampled_image;

		// Define subresource range:
		ImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = ImageAspectFlags::color_bit;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.layerCount = 1;
		subresourceRange.levelCount = static_cast<uint32_t>(math::Floor(math::Log2(math::Max(m_width, m_height)))) + 1;	// mip levels

		// Create image:
		ImageUsageFlag usageFlags = ImageUsageFlags::transfer_src_bit | ImageUsageFlags::transfer_dst_bit | ImageUsageFlags::sampled_bit;
		ImageCreateFlag imageFlags = 0;
		MemoryPropertyFlag memoryFlags = MemoryPropertyFlags::device_local_bit;
		ImageViewType viewType = ImageViewTypes::view_type_2d;
		DeviceQueue queue = Context::GetLogicalDevice()->GetTransferQueue();
		CreateImage(subresourceRange, m_format, usageFlags, imageFlags, memoryFlags, viewType, queue);

		NAME_VK_IMAGE(m_pImage->GetVkImage(), "SampleTexture2d " + m_name);
	}
	StagingBuffer* SampleTexture2d::StageData(void* data)
    {
		// Upload: data -> pStagingBuffer
		uint64_t bufferSize = m_channels * m_width * m_height * BytesPerChannel(m_format);
		StagingBuffer* pStagingBuffer = new StagingBuffer(bufferSize, m_name);
		pStagingBuffer->SetData(data, bufferSize);
		return pStagingBuffer;
    }
	void SampleTexture2d::Upload(StagingBuffer* pStagingBuffer)
	{
		const DeviceQueue& transferQueue = Context::GetLogicalDevice()->GetTransferQueue();
		const DeviceQueue& graphicsQueue = Context::GetLogicalDevice()->GetGraphicsQueue();
		if (transferQueue.queue != graphicsQueue.queue)
		{
			VkCommandBuffer transferCommandBuffer = SingleTimeCommand::BeginCommand(transferQueue);
			VkCommandBuffer graphicsCommandBuffer = SingleTimeCommand::BeginCommand(graphicsQueue);
			RecordGpuCommands(transferCommandBuffer, graphicsCommandBuffer, pStagingBuffer);
			SingleTimeCommand::EndLinkedCommands(transferQueue, graphicsQueue, PipelineStages::transfer);
		}
		else
		{
			VkCommandBuffer commandBuffer = SingleTimeCommand::BeginCommand(graphicsQueue);
			RecordGpuCommands(commandBuffer, commandBuffer, pStagingBuffer);
			SingleTimeCommand::EndCommand(graphicsQueue);
		}
	}
	void SampleTexture2d::RecordGpuCommands(VkCommandBuffer transferCommandBuffer, VkCommandBuffer graphicsCommandBuffer, StagingBuffer* pStagingBuffer)
	{
		// Transition 0: Layout: undefined->dstTransfer, Queue: transfer
		{
			ImageLayout newLayout = ImageLayouts::transfer_dst_optimal;
			PipelineStage srcStage = PipelineStages::topOfPipe;
			PipelineStage dstStage = PipelineStages::transfer;
			AccessMask srcAccessMask = AccessMasks::Transfer::none;
			AccessMask dstAccessMask = AccessMasks::Transfer::transferWrite;
			m_pImage->TransitionLayout(transferCommandBuffer, newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
		}

		// Upload: pStagingBuffer -> texture
		pStagingBuffer->UploadToTexture(transferCommandBuffer, this, m_pImage->GetImageSubresourceRange().layerCount);

		// Transition 1: Layout: transfer->shaderRead
		// With mipmapping: Queue: graphics
		if (m_pImage->GetImageSubresourceRange().levelCount > 1)
			m_pImage->GenerateMipmaps(graphicsCommandBuffer, m_pImage->GetImageSubresourceRange().levelCount);
		// Without mipmapping: Queue: transfer
		else
		{
			ImageLayout newLayout = ImageLayouts::shader_read_only_optimal;
			PipelineStage srcStage = PipelineStages::transfer;
			PipelineStage dstStage = PipelineStages::fragmentShader;
			AccessMask srcAccessMask = AccessMasks::Transfer::transferWrite;
			AccessMask dstAccessMask = AccessMasks::FragmentShader::shaderRead;
			m_pImage->TransitionLayout(transferCommandBuffer, newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
		}
	}
}