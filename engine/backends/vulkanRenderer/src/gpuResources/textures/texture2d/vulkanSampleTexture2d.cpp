#include "vulkanSampleTexture2d.h"
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
	SampleTexture2d::SampleTexture2d(VkFormat format, int width, int height, void* data)
	{
		Init(format, width, height);
		if (data)
			SetData(data);
		else
			ClearAndPrepareForSampling();
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
		UploadAndPrepareForSampling(pStagingBuffer.get());
	}
	


	// Private methods:
	void SampleTexture2d::Init(VkFormat format, int width, int height)
	{
		if (!IsValidImageFormat(format))
			throw std::runtime_error("SampleTexture2d::Init(...) failed. Unsupported format: " + std::to_string(static_cast<int>(format)));

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
		subresourceRange.layerCount = 1;
		subresourceRange.levelCount = static_cast<uint32_t>(math::Floor(math::Log2(math::Max(m_width, m_height)))) + 1;	// mip levels

		// Create image:
		VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		VkImageCreateFlags imageFlags = 0;
		VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
		DeviceQueue queue = Context::GetLogicalDevice()->GetTransferQueue();
		CreateImage(subresourceRange, m_format, usageFlags, imageFlags, memoryFlags, viewType, queue);

		SetDebugName("SampleTexture2d");
	}
}