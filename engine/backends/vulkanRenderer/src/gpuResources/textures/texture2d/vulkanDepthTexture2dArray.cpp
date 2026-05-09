#include "vulkanDepthTexture2dArray.h"
#include "logger.h"
#include "vmaImage.h"
#include "vulkanContext.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"



namespace vulkanRendererBackend
{
	// Constructor/Desctructor:
	DepthTexture2dArray::DepthTexture2dArray(Format format, int width, int height, uint32_t layerCount)
	{
		if (!IsDepthFormat(format))
			throw std::runtime_error("DepthTexture2dArray::DepthTexture2dArray(...) failed. Unsupported format: " + std::to_string(static_cast<int>(format)));

		m_width = width;
		m_height = height;
		m_channels = GetChannelCount(format);
		m_format = format;
		m_vkDescriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

		// Define subresource range:
		VkImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.layerCount = layerCount;
		subresourceRange.levelCount = 1; // no mipmapping for shadow textures.

		// Create image:
		VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		VkImageCreateFlags imageFlags = 0;
		MemoryPropertyFlag memoryFlags = MemoryPropertyFlags::device_local_bit;
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
		DeviceQueue queue = Context::GetLogicalDevice()->GetGraphicsQueue();
		CreateImage(subresourceRange, m_format, usageFlags, imageFlags, memoryFlags, viewType, queue);

		NAME_VK_OBJECT(m_pImage->GetVkImage(), "DepthTexture2dArray");
	}
	DepthTexture2dArray::~DepthTexture2dArray()
	{

	}



	// Public method:
	void DepthTexture2dArray::SetData(void* data)
	{
		LOG_WARN("Setting data of a DepthTexture2dArray manually is not allowed!");
	}
}