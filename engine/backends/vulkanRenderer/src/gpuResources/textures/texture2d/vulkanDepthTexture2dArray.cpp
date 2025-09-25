#include "vulkanDepthTexture2dArray.h"
#include "logger.h"
#include "vmaImage.h"
#include "vulkanContext.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"



namespace vulkanRendererBackend
{
	// Constructor/Desctructor:
	DepthTexture2dArray::DepthTexture2dArray(const std::string& name, Format format, int width, int height, uint32_t layerCount)
	{
		if (!IsDepthFormat(format))
			throw std::runtime_error("DepthTexture2dArray '" + name + "' uses unsuported format: " + std::to_string(static_cast<int>(format)));

		m_name = name;
		m_width = width;
		m_height = height;
		m_channels = GetChannelCount(format);
		m_format = format;
		m_descriptorType = DescriptorTypes::sampled_image;

		// Define subresource range:
		ImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = ImageAspectFlags::depth_bit;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.layerCount = layerCount;
		subresourceRange.levelCount = 1; // no mipmapping for shadow textures.

		// Create image:
		ImageUsageFlag usageFlags = ImageUsageFlags::depth_stencil_attachment_bit | ImageUsageFlags::sampled_bit;
		ImageCreateFlag imageFlags = 0;
		MemoryPropertyFlag memoryFlags = MemoryPropertyFlags::device_local_bit;
		ImageViewType viewType = ImageViewTypes::view_type_2d_array;
		DeviceQueue queue = Context::GetLogicalDevice()->GetGraphicsQueue();
		CreateImage(subresourceRange, m_format, usageFlags, imageFlags, memoryFlags, viewType, queue);

		// Although the image is in the layout undefined, the shadow renderpass automatically transitions it to shader read after rendering the shadow maps into it.
		// The descriptorSet that binds the shadowMap to the fragment shaders of the forward renderpass needs to read the correct VkImageLayout.
		// Thus we have to manually set the layout to shader read.
		// Ember::ToDo: This feels kinda like a hack, maybe due to bad code architecture. I will try to make this better some time.
		m_pImage->SetLayout(ImageLayouts::shader_read_only_optimal);

		NAME_VK_IMAGE(m_pImage->GetVkImage(), "DepthTexture2dArray " + m_name);
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