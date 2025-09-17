#include "vulkanRenderTexture2d.h"
#include "logger.h"
#include "vmaImage.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"



namespace vulkanRendererBackend
{
	// Constructor/Desctructor:
	RenderTexture2d::RenderTexture2d(const std::string& name, VkFormat format, int width, int height, VkImageUsageFlags usageFlags)
	{
		if (!IsValidImageFormat(format))
			throw std::runtime_error("RenderTexture2d '" + name + "' uses unsuported format: " + std::to_string(static_cast<int>(format)));

		m_name = name;
		m_width = width;
		m_height = height;
		m_channels = GetChannelCount(format);
		m_format = format;
		m_descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

		// Define subresource range:
		VkImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.layerCount = 1;
		subresourceRange.levelCount = 1;

		// Create image:
		if (usageFlags == 0)	// Default usage flags:
			usageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		VkImageCreateFlags imageFlags = 0;
		VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
		DeviceQueue queue = Context::logicalDevice.GetTransferQueue();
		CreateImage(subresourceRange, m_format, usageFlags, imageFlags, memoryFlags, viewType, queue);

		NAME_VK_IMAGE(m_pImage->GetVkImage(), "RenderTexture2d " + m_name);
	}
	RenderTexture2d::~RenderTexture2d()
	{

	}
}