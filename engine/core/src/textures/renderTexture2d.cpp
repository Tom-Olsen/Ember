#include "renderTexture2d.h"
#include "vmaImage.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"



namespace emberEngine
{
	using namespace vulkanBackend;



	// Constructor/Desctructor:
	RenderTexture2d::RenderTexture2d(const std::string& name, VkFormat format, int width, int height, VkImageUsageFlags usageFlags)
	{
		m_name = name;
		m_width = width;
		m_height = height;
		m_channels = 4;
		m_descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

		// Define subresource range:
		VkImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = 1;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.layerCount = 1;

		// Create image:
		if (usageFlags == 0)	// Default usage flags:
			usageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		VkImageCreateFlags imageFlags = 0;
		VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
		DeviceQueue queue = Context::logicalDevice.GetTransferQueue();
		CreateImage(subresourceRange, format, usageFlags, imageFlags, memoryFlags, viewType, queue);
		NAME_VK_IMAGE(m_pImage->GetVkImage(), "RenderTexture2d " + m_name);
	}
	RenderTexture2d::~RenderTexture2d()
	{

	}



	// Pritvat methods:
	StagingBuffer* RenderTexture2d::Init(const std::string& name, VkFormat format, const std::filesystem::path& path)
	{
		LOG_WARN("RenderTexture2d::Init(...) not implemented yet.");
		return nullptr;
	}



	// Public methods:
	void RenderTexture2d::RecordGpuCommands(VkCommandBuffer& transferCommandBuffer, VkCommandBuffer& graphicsCommandBuffer, StagingBuffer* pStagingBuffer)
	{
		LOG_WARN("RenderTexture2d::RecordGpuCommands(...) not implemented yet.");
	}
}