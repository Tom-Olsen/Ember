#include "vulkanTextureCube.h"
#include "vmaImage.h"
#include "vulkanDeviceQueue.h"



namespace vulkanRendererBackend
{
	// Constructor/Destructor:
    TextureCube::TextureCube()
	{
		m_depth = 1;
	}
	TextureCube::~TextureCube()
	{

	}



	// Protected methods:
	void TextureCube::CreateImage(VkImageSubresourceRange& subresourceRange, VkFormat format, VkImageUsageFlags usageFlags, VkImageCreateFlags imageFlags, VkMemoryPropertyFlags memoryFlags, VkImageViewType viewType, const DeviceQueue& queue)
	{
		imageFlags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		VkImageType imageType = VK_IMAGE_TYPE_2D;
		CreateImageBase(imageType, subresourceRange, format, usageFlags, imageFlags, memoryFlags, viewType, queue);
	}
}