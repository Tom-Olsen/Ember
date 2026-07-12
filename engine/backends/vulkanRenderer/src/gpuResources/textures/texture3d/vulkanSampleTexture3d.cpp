#include "vulkanSampleTexture3d.h"
#include "assetLoader.h"
#include "emberMath.h"
#include "vmaImage.h"
#include "vulkanAccessMask.h"
#include "vulkanContext.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include "vulkanSingleTimeCommand.h"
#include "vulkanStagingBuffer.h"
#include <memory>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Desctructor:
	SampleTexture3d::SampleTexture3d(VkFormat format, int width, int height, int depth, void* data)
	{
		Init(format, width, height, depth);
		if (data)
			SetData(data);
		else
		{
			VkImageLayout newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			VkPipelineStageFlags2 srcStage = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
			VkPipelineStageFlags2 dstStage = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
			AccessMask srcAccessMask = AccessMasks::TopOfPipe::none;
			AccessMask dstAccessMask = AccessMasks::FragmentShader::shaderRead;
			m_pImage->TransitionLayout(newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
		}
	}
	SampleTexture3d::~SampleTexture3d()
	{

	}



	// Movable:
	SampleTexture3d::SampleTexture3d(SampleTexture3d&& other) noexcept = default;
	SampleTexture3d& SampleTexture3d::operator=(SampleTexture3d&& other) noexcept = default;



	void SampleTexture3d::SetData(void* data)
	{
		std::unique_ptr<StagingBuffer> pStagingBuffer = std::unique_ptr<StagingBuffer>(StageData(data));
		Upload(pStagingBuffer.get());
	}



	// Private methods:
	void SampleTexture3d::Init(VkFormat format, int width, int height, int depth)
	{
		if (!IsValidImageFormat(format))
			throw std::runtime_error("SampleTexture3d::Init(...) failed. Unsupported format: " + std::to_string(static_cast<int>(format)));

		m_width = width;
		m_height = height;
		m_depth = depth;
		m_channels = GetChannelCount(format);
		m_format = format;
		m_vkDescriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

		// Define subresource range:
		VkImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.layerCount = 1;
		subresourceRange.levelCount = static_cast<uint32_t>(math::Floor(math::Log2(math::Max(m_width, math::Max(m_height, m_depth))))) + 1;

		// Create image:
		VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		VkImageCreateFlags imageFlags = 0;
		VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_3D;
		DeviceQueue queue = Context::GetLogicalDevice()->GetTransferQueue();
		CreateImage(subresourceRange, m_format, usageFlags, imageFlags, memoryFlags, viewType, queue);

		SetDebugName("SampleTexture3d");
	}
	StagingBuffer* SampleTexture3d::StageData(void* data)
	{
		// Upload: data -> pStagingBuffer
		uint64_t bufferSize = m_channels * m_width * m_height * m_depth * BytesPerChannel(m_format);
		StagingBuffer* pStagingBuffer = new StagingBuffer(bufferSize);
		pStagingBuffer->SetData(data, bufferSize);
		return pStagingBuffer;
	}
	void SampleTexture3d::Upload(StagingBuffer* pStagingBuffer)
	{
		const DeviceQueue& transferQueue = Context::GetLogicalDevice()->GetTransferQueue();
		const DeviceQueue& graphicsQueue = Context::GetLogicalDevice()->GetGraphicsQueue();
		if (transferQueue.queue != graphicsQueue.queue)
		{
			VkCommandBuffer transferCommandBuffer = SingleTimeCommand::BeginCommand(transferQueue);
			VkCommandBuffer graphicsCommandBuffer = SingleTimeCommand::BeginCommand(graphicsQueue);
			RecordUploadAndPrepareForSamplingCommands(transferCommandBuffer, graphicsCommandBuffer, pStagingBuffer);
			SingleTimeCommand::EndLinkedCommands(transferQueue, graphicsQueue, VK_PIPELINE_STAGE_2_TRANSFER_BIT);
		}
		else
		{
			VkCommandBuffer commandBuffer = SingleTimeCommand::BeginCommand(graphicsQueue);
			RecordUploadAndPrepareForSamplingCommands(commandBuffer, commandBuffer, pStagingBuffer);
			SingleTimeCommand::EndCommand(graphicsQueue);
		}
	}
	void SampleTexture3d::RecordUploadAndPrepareForSamplingCommands(VkCommandBuffer transferCommandBuffer, VkCommandBuffer graphicsCommandBuffer, StagingBuffer* pStagingBuffer)
	{
		// Transition 0: Layout: undefined->dstTransfer, Queue: transfer
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

		// Transition 1: Layout: transfer->shaderRead
		// With mipmapping: Queue: graphics
		if (m_pImage->GetImageSubresourceRange().levelCount > 1)
			m_pImage->GenerateMipmaps(graphicsCommandBuffer, m_pImage->GetImageSubresourceRange().levelCount);
		// Without mipmapping: Queue: transfer
		else
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