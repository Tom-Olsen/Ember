#include "vulkanSampleTextureCube.h"
#include "assetLoader.h"
#include "vmaImage.h"
#include "vulkanAccessMask.h"
#include "vulkanContext.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include "vulkanSingleTimeCommand.h"
#include "vulkanStagingBuffer.h"
#include <array>
#include <memory>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	SampleTextureCube::SampleTextureCube(VkFormat format, int width, int height, void* data)
	{
		Init(format, width, height);
		if (data)
			SetData(data);
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
	void SampleTextureCube::Init(VkFormat format, int width, int height)
	{
		if (!IsValidImageFormat(format))
			throw std::runtime_error("SampleTextureCube::Init(...) failed. Unsupported format: " + std::to_string(static_cast<int>(format)));

		m_width = width;
		m_height = height;
		m_channels = GetChannelCount(format);
		m_format = format;
		m_vkDescriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

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
		DeviceQueue queue = Context::GetLogicalDevice()->GetTransferQueue();
		CreateImage(subresourceRange, m_format, usageFlags, imageFlags, memoryFlags, viewType, queue);

		SetDebugName("SampleTextureCube");
	}

	StagingBuffer* SampleTextureCube::StageData(void* data)
	{
		// Upload: data -> pStagingBuffer
		uint64_t bufferSize = 6 * m_channels * m_width * m_height * BytesPerChannel(m_format);
		StagingBuffer* pStagingBuffer = new StagingBuffer(bufferSize);
		pStagingBuffer->SetData(data, bufferSize);
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
			SingleTimeCommand::EndLinkedCommands(transferQueue, graphicsQueue, VK_PIPELINE_STAGE_2_TRANSFER_BIT);
		}
		else
		{
			VkCommandBuffer commandBuffer = SingleTimeCommand::BeginCommand(graphicsQueue);
			RecordGpuCommands(commandBuffer, commandBuffer, pStagingBuffer);
			SingleTimeCommand::EndCommand(graphicsQueue);
		}
	}
	void SampleTextureCube::RecordGpuCommands(VkCommandBuffer transferCommandBuffer, VkCommandBuffer graphicsCommandBuffer, StagingBuffer* pStagingBuffer)
	{
		// Transition0: Layout: undefined->transfer, Queue: transfer
		{
			VkImageLayout newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			VkPipelineStageFlags2 srcStage = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
			VkPipelineStageFlags2 dstStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
			AccessMask srcAccessMask = AccessMasks::TopOfPipe::none;
			AccessMask dstAccessMask = AccessMasks::Transfer::transferWrite;
			m_pImage->TransitionLayout(transferCommandBuffer, newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
		}

		// Upload: pStagingBuffer -> texture
		pStagingBuffer->UploadToTexture(transferCommandBuffer, this, m_pImage->GetImageSubresourceRange().layerCount);

		// Transition1: Layout: transfer->shaderRead
		{
			VkImageLayout newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			VkPipelineStageFlags2 srcStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
			VkPipelineStageFlags2 dstStage = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
			AccessMask srcAccessMask = AccessMasks::Transfer::transferWrite;
			AccessMask dstAccessMask = AccessMasks::FragmentShader::shaderRead;
			m_pImage->TransitionLayout(transferCommandBuffer, newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
		}
	}
}