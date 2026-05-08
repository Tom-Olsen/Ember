#include "vulkanRenderTexture2d.h"
#include "logger.h"
#include "vmaImage.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include "vulkanLogicalDevice.h"



namespace vulkanRendererBackend
{
	// Public method:
	// Constructor/Desctructor:
	RenderTexture2d::RenderTexture2d(Format format, int width, int height)
	{
		if (!IsValidImageFormat(format))
			throw std::runtime_error("RenderTexture2d::RenderTexture2d(...) failed. Unsupported format: " + std::to_string(static_cast<int>(format)));

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
		subresourceRange.levelCount = 1;

		// Create image:
		ImageUsageFlag usageFlags = ImageUsageFlags::transfer_src_bit | ImageUsageFlags::transfer_dst_bit | ImageUsageFlags::storage_bit | ImageUsageFlags::sampled_bit | ImageUsageFlags::color_attachment_bit;
		ImageCreateFlag imageFlags = 0;
		MemoryPropertyFlag memoryFlags = MemoryPropertyFlags::device_local_bit;
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
		DeviceQueue queue = Context::GetLogicalDevice()->GetTransferQueue();
		CreateImage(subresourceRange, m_format, usageFlags, imageFlags, memoryFlags, viewType, queue);

		NAME_VK_OBJECT(m_pImage->GetVkImage(), "RenderTexture2d");
	}
	RenderTexture2d::~RenderTexture2d()
	{

	}



	// Movable:
	RenderTexture2d::RenderTexture2d(RenderTexture2d&& other) noexcept = default;
	RenderTexture2d& RenderTexture2d::operator=(RenderTexture2d&& other) noexcept = default;



	void RenderTexture2d::SetData(void* data)
	{
		LOG_WARN("Setting data of a RenderTexture2d manually is not allowed!");
	}
}