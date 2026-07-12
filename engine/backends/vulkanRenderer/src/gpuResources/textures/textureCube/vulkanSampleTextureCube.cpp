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
		else
			ClearAndPrepareForSampling();
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
		UploadAndPrepareForSampling(pStagingBuffer.get());
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

}