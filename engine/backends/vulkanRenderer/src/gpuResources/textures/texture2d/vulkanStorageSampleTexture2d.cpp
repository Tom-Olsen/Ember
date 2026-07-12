#include "vulkanStorageSampleTexture2d.h"
#include "assetLoader.h"
#include "vmaImage.h"
#include "vulkanAccessMask.h"
#include "vulkanContext.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include "vulkanStagingBuffer.h"



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Desctructor:
	StorageSampleTexture2d::StorageSampleTexture2d(VkFormat format, int width, int height, void* data)
	{
		Init(format, width, height);
		if (data)
			SetData(data);
		else
			PrepareForStorage();
	}
	StorageSampleTexture2d::~StorageSampleTexture2d()
	{

	}



	// Movable:
	StorageSampleTexture2d::StorageSampleTexture2d(StorageSampleTexture2d&& other) noexcept = default;
	StorageSampleTexture2d& StorageSampleTexture2d::operator=(StorageSampleTexture2d&& other) noexcept = default;



	void StorageSampleTexture2d::SetData(void* data)
	{
		std::unique_ptr<StagingBuffer> pStagingBuffer = std::unique_ptr<StagingBuffer>(StageData(data));
		UploadAndPrepareForStorage(pStagingBuffer.get());
	}



	// Private methods:
	void StorageSampleTexture2d::Init(VkFormat format, int width, int height)
	{
		if (!IsValidImageFormat(format))
			throw std::runtime_error("StorageSampleTexture2d::Init(...) failed. Unsupported format: " + std::to_string(static_cast<int>(format)));

		m_width = width;
		m_height = height;
		m_channels = GetChannelCount(format);
		m_format = format;
		m_vkDescriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

		// Define subresource range:
		VkImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.layerCount = 1;
		subresourceRange.levelCount = 1;	// no mipmapping for storage images.

		// Create image:
		VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		VkImageCreateFlags imageFlags = 0;
		VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
		DeviceQueue queue = Context::GetLogicalDevice()->GetTransferQueue();
		CreateImage(subresourceRange, m_format, usageFlags, imageFlags, memoryFlags, viewType, queue);

		SetDebugName("StorageSampleTexture2d");
	}
}