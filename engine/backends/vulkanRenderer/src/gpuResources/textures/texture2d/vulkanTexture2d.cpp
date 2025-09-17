#include "vulkanTexture2d.h"
#include "vmaImage.h"
#include "vulkanDeviceQueue.h"



namespace vulkanRendererBackend
{
	// Virtual Destructor:
	Texture2d::Texture2d()
	{
		m_depth = 1;
	}
	Texture2d::~Texture2d()
	{

	}



	// Protected methods:
	void Texture2d::CreateImage(VkImageSubresourceRange& subresourceRange, VkFormat format, VkImageUsageFlags usageFlags, VkImageCreateFlags imageFlags, VkMemoryPropertyFlags memoryFlags, VkImageViewType viewType, const DeviceQueue& queue)
	{
		VkImageType imageType = VK_IMAGE_TYPE_2D;
		CreateImageBase(imageType, subresourceRange, format, usageFlags, imageFlags, memoryFlags, viewType, queue);
	}
}