#include "vulkanGBufferTexture2d.h"
#include "logger.h"
#include "vulkanContext.h"
#include "vulkanLogicalDevice.h"
#include <stdexcept>
#include <string>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	GBufferTexture2d::GBufferTexture2d(VkFormat format, int width, int height)
	{
		if (!IsValidImageFormat(format))
			throw std::runtime_error("GBufferTexture2d::GBufferTexture2d(...) failed. Unsupported format: " + std::to_string(static_cast<int>(format)));

		m_width = width;
		m_height = height;
		m_channels = GetChannelCount(format);
		m_format = format;
		m_vkDescriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

		VkImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.layerCount = 1;
		subresourceRange.levelCount = 1;

		VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		VkImageCreateFlags imageFlags = 0;
		VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
		DeviceQueue queue = Context::GetLogicalDevice()->GetGraphicsQueue();
		CreateImage(subresourceRange, m_format, usageFlags, imageFlags, memoryFlags, viewType, queue);

		SetDebugName("GBufferTexture2d");
	}
	GBufferTexture2d::~GBufferTexture2d()
	{

	}



	// Public methods:
	void GBufferTexture2d::SetData(void* data)
	{
		LOG_WARN("Setting data of a GBufferTexture2d manually is not allowed!");
	}
}