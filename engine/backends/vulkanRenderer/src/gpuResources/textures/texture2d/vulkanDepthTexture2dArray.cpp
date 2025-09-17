#include "vulkanDepthTexture2dArray.h"
#include "logger.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"



namespace vulkanRendererBackend
{
	// Constructor/Desctructor:
	DepthTexture2dArray::DepthTexture2dArray(const std::string& name, VkFormat format, int width, int height, uint32_t layerCount)
	{
		if (!IsDepthFormat(format))
			throw std::runtime_error("DepthTexture2dArray '" + name + "' uses unsuported format: " + std::to_string(static_cast<int>(format)));

		m_name = name;
		m_width = width;
		m_height = height;
		m_channels = GetChannelCount(format);
		m_format = format;
		m_descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

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
		VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
		DeviceQueue queue = Context::logicalDevice.GetGraphicsQueue();
		CreateImage(subresourceRange, m_format, usageFlags, imageFlags, memoryFlags, viewType, queue);

		// Although the image is in the layout undefined, the shadow renderpass automatically transitions it to shader read after rendering the shadow maps into it.
		// The descriptorSet that binds the shadowMap to the fragment shaders of the forward renderpass needs to read the correct VkImageLayout.
		// Thus we have to manually set the layout to shader read.
		// Ember::ToDo: This feels kinda like a hack, maybe due to bad code architecture. I will try to make this better some time.
		m_pImage->SetLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		NAME_VK_IMAGE(m_pImage->GetVkImage(), "DepthTexture2dArray " + m_name);
	}
	DepthTexture2dArray::~DepthTexture2dArray()
	{

	}
}