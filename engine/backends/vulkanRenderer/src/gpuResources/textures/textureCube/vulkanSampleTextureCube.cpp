#include "vulkanSampleTextureCube.h"
#include "assetLoader.h"
#include "vmaImage.h"
#include "vulkanAccessMask.h"
#include "vulkanContext.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include "vulkanPipelineStage.h"
#include "vulkanSingleTimeCommand.h"
#include "vulkanStagingBuffer.h"
#include <array>
#include <memory>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	SampleTextureCube::SampleTextureCube(const std::string& name, Format format, int width, int height)
	{
		Init(name, format, width, height);
	}
	SampleTextureCube::SampleTextureCube(const std::string& name, Format format, int width, int height, void* data)
	{
		Init(name, format, width, height);
		SetData(data);
	}
	SampleTextureCube::SampleTextureCube(const std::string& name, Format format, const std::filesystem::path& path)
    {
        // Check if folder exists:
		if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
			throw std::runtime_error("Given path '" + path.string() + "' does not exist");

		// Cube face names:
		static const std::array<std::string, 6> directions = { "px", "nx", "py", "ny", "pz", "nz" };

		// Collect file paths:
		std::vector<std::filesystem::path> filePaths(6);
		for (uint32_t i = 0; i < 6; i++)
		{
			bool found = false;
			for (const auto& entry : std::filesystem::directory_iterator(path))
			{
				if (entry.is_regular_file() && entry.path().stem() == directions[i])
				{
					filePaths[i] = entry.path();
					found = true;
					break;
				}
			}
			if (!found)
				throw std::runtime_error("Missing cubemap face: " + directions[i]);
		}

		// Load all 6 images and append pixel data continuously
		std::vector<std::byte> data;
		int width, height;
		int channels = GetChannelCount(format);
		for (size_t i = 0; i < filePaths.size(); i++)
		{
			emberAssetLoader::Image imageAsset = emberAssetLoader::LoadImageFile(filePaths[i], channels);
			if (i == 0)
			{
				width = imageAsset.width;
				height = imageAsset.height;
			}
			else
			{
				if (imageAsset.width != width || imageAsset.height != height)
					throw std::runtime_error("Cubemap face size mismatch at: " + filePaths[i].string());
			}
			data.insert(data.end(), imageAsset.pixels.begin(), imageAsset.pixels.end());
		}

        // Upload data:
		Init(name, format, width, height);
		SetData(data.data());
    }
	SampleTextureCube::~SampleTextureCube()
	{

	}



	// Movable:
	SampleTextureCube::SampleTextureCube(SampleTextureCube&& other) noexcept = default;
	SampleTextureCube& SampleTextureCube::operator=(SampleTextureCube&& other) noexcept = default;



	void SampleTextureCube::SetData(void* data)
	{
		std::unique_ptr<StagingBuffer> pStagingBuffer = std::unique_ptr<StagingBuffer>(StageData(data));
		Upload(pStagingBuffer.get());
	}
	



	// Private methods:
	void SampleTextureCube::Init(const std::string& name, Format format, int width, int height)
	{
		if (!IsValidImageFormat(format))
			throw std::runtime_error("SampleTextureCube '" + name + "' uses unsuported format: " + std::to_string(static_cast<int>(format)));

		m_name = name;
		m_width = width;
		m_height = height;
		m_channels = GetChannelCount(format);
		m_format = format;
		m_descriptorType = DescriptorTypes::sampled_image;
		NAME_VK_IMAGE(m_pImage->GetVkImage(), "SampleTextureCube " + m_name);
	}

	StagingBuffer* SampleTextureCube::StageData(void* data)
	{
		// Upload: data -> pStagingBuffer
		uint64_t bufferSize = 6 * m_channels * m_width * m_height * BytesPerChannel(m_format);
		StagingBuffer* pStagingBuffer = new StagingBuffer(bufferSize, m_name);
		pStagingBuffer->SetData(data, bufferSize);

		// Define subresource range:
		ImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = ImageAspectFlags::color_bit;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = 1;	// mipmapping makes no sense for skyboxes.
		subresourceRange.layerCount = 6;

		// Create image:
		ImageUsageFlag usageFlags = ImageUsageFlags::transfer_src_bit | ImageUsageFlags::transfer_dst_bit | ImageUsageFlags::sampled_bit;
		ImageCreateFlag imageFlags = ImageCreateFlags::cube_compatible_bit;
		MemoryPropertyFlag memoryFlags = MemoryPropertyFlags::device_local_bit;
		ImageViewType viewType = ImageViewTypes::view_type_cube;
		DeviceQueue queue = Context::GetLogicalDevice()->GetTransferQueue();
		CreateImage(subresourceRange, m_format, usageFlags, imageFlags, memoryFlags, viewType, queue);

		return pStagingBuffer;
	}
	void SampleTextureCube::Upload(StagingBuffer* pStagingBuffer)
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
		delete pStagingBuffer;
	}
	void SampleTextureCube::RecordGpuCommands(VkCommandBuffer transferCommandBuffer, VkCommandBuffer graphicsCommandBuffer, StagingBuffer* pStagingBuffer)
	{
		// Transition0: Layout: undefined->transfer, Queue: transfer
		{
			ImageLayout newLayout = ImageLayouts::transfer_dst_optimal;
			PipelineStage srcStage = PipelineStages::topOfPipe;
			PipelineStage dstStage = PipelineStages::transfer;
			AccessMask srcAccessMask = AccessMasks::TopOfPipe::none;
			AccessMask dstAccessMask = AccessMasks::Transfer::transferWrite;
			m_pImage->TransitionLayout(transferCommandBuffer, newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
		}

		// Upload: pStagingBuffer -> texture
		pStagingBuffer->UploadToTexture(transferCommandBuffer, this, m_pImage->GetImageSubresourceRange().layerCount);

		// Transition1: Layout: transfer->shaderRead
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