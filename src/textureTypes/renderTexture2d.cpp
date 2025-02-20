#include "renderTexture2d.h"
#include "logger.h"
#include "stb_image.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanContext.h"



namespace emberEngine
{
	// Constructor/Desctructor:
	RenderTexture2d::RenderTexture2d(VulkanContext* pContext, const std::string& name, VkFormat format, int width, int height)
	{
		m_pContext = pContext;
		m_type = Type::sample;
		m_name = name;
		m_width = width;
		m_height = height;
		m_channels = STBI_rgb_alpha;	// 4 channels
		m_descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

		// Define subresource range:
		VkImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = 1;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.layerCount = 1;

		// Create image:
		VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		VkImageCreateFlags imageFlags = 0;
		VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
		VulkanQueue queue = m_pContext->pLogicalDevice->GetTransferQueue();
		m_pImage = std::unique_ptr<VmaImage>(CreateImage(subresourceRange, format, usageFlags, imageFlags, memoryFlags, viewType, queue));

		//// Transition: Layout: undefined->transfer, Queue: transfer
		//VkImageLayout newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		//VkPipelineStageFlags2 srcStage = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
		//VkPipelineStageFlags2 dstStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
		//VkAccessFlags2 srcAccessMask = VK_ACCESS_2_NONE;
		//VkAccessFlags2 dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
		//m_pImage->TransitionLayout(newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
	}
	RenderTexture2d::~RenderTexture2d()
	{

	}
}