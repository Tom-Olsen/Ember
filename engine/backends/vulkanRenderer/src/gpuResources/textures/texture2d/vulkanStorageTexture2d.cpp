#include "vulkanStorageTexture2d.h"
#include "assetLoader.h"
#include "vmaImage.h"
#include "vulkanAccessMask.h"
#include "vulkanContext.h"
#include "vulkanDescriptorType.h"
#include "vulkanImageAspectFlag.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include "vulkanPipelineStage.h"
#include "vulkanStagingBuffer.h"



namespace vulkanRendererBackend
{
	// Constructor/Desctructor:
	StorageTexture2d::StorageTexture2d(const std::string& name, Format format, int width, int height)
	{
		Init(name, format, width, height);
	}
	StorageTexture2d::StorageTexture2d(const std::string& name, Format format, int width, int height, void* data)
	{
		Init(name, format, width, height);
		SetData(data);
	}
	StorageTexture2d::StorageTexture2d(const std::string& name, Format format, const std::filesystem::path& path)
	{
		emberAssetLoader::Image imageAsset = emberAssetLoader::LoadImageFile(path, GetChannelCount(format));
		Init(name, format, imageAsset.width, imageAsset.height);
		SetData((void*)imageAsset.pixels.data());
	}
	StorageTexture2d::~StorageTexture2d()
	{

	}



	// Public methods:
	void StorageTexture2d::SetData(void* data)
	{
		std::unique_ptr<StagingBuffer> pStagingBuffer = std::unique_ptr<StagingBuffer>(StageData(data));
		Upload(pStagingBuffer.get());
	}



	// Private methods:
	void StorageTexture2d::Init(const std::string& name, Format format, int width, int height)
	{
		if (!IsValidImageFormat(format))
			throw std::runtime_error("StorageTexture2d '" + name + "' uses unsuported format: " + std::to_string(static_cast<int>(format)));

		m_name = name;
		m_width = width;
		m_height = height;
		m_channels = GetChannelCount(format);
		m_format = format;
		m_descriptorType = DescriptorTypes::storage_image;
		NAME_VK_IMAGE(m_pImage->GetVkImage(), "StorageTexture2d " + m_name);
	}
	StagingBuffer* StorageTexture2d::StageData(void* data)
	{
		// Upload: data -> pStagingBuffer
		uint64_t bufferSize = m_channels * m_width * m_height * BytesPerChannel(m_format);
		StagingBuffer* pStagingBuffer = new StagingBuffer(bufferSize, m_name);
		pStagingBuffer->SetData(data, bufferSize);

		// Define subresource range:
		ImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = ImageAspectFlags::color_bit;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.layerCount = 1;
		subresourceRange.levelCount = 1;

		// Create image:
		ImageUsageFlag usageFlags = ImageUsageFlags::transfer_dst_bit | ImageUsageFlags::storage_bit;
		ImageCreateFlag imageFlags = 0;
		MemoryPropertyFlag memoryFlags = MemoryPropertyFlags::device_local_bit;
		ImageViewType viewType = ImageViewTypes::view_type_2d;
		DeviceQueue queue = Context::GetLogicalDevice()->GetTransferQueue();
		CreateImage(subresourceRange, m_format, usageFlags, imageFlags, memoryFlags, viewType, queue);

		return pStagingBuffer;
	}
	void StorageTexture2d::Upload(StagingBuffer* pStagingBuffer)
	{
		// Transition 0: Layout: undefined->transfer, Queue: transfer
		{
			ImageLayout newLayout = ImageLayouts::transfer_dst_optimal;
			PipelineStage srcStage = PipelineStages::topOfPipe;
			PipelineStage dstStage = PipelineStages::transfer;
			AccessMask srcAccessMask = AccessMasks::TopOfPipe::none;
			AccessMask dstAccessMask = AccessMasks::Transfer::transferWrite;
			m_pImage->TransitionLayout(newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
		}

		// Upload: pStagingBuffer -> texture
		pStagingBuffer->UploadToTexture(Context::GetLogicalDevice()->GetTransferQueue(), this, m_pImage->GetImageSubresourceRange().layerCount);

		// Transition 1: Layout: transfer->general, Queue: transfer->compute
		{
			ImageLayout newLayout = ImageLayouts::general;
			PipelineStage srcStage = PipelineStages::transfer;
			PipelineStage dstStage = PipelineStages::computeShader;
			AccessMask srcAccessMask = AccessMasks::Transfer::transferWrite;
			AccessMask dstAccessMask = AccessMasks::ComputeShader::memoryRead | AccessMasks::ComputeShader::memoryWrite;
			m_pImage->TransitionLayout(newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
		}
	}
}