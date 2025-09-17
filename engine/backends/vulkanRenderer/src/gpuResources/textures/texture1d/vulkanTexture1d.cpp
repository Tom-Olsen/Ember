#include "vulkanTexture1d.h"
#include "vmaImage.h"
#include "vulkanDeviceQueue.h"



namespace vulkanRendererBackend
{
	// Virtual Destructor:
	Texture1d::Texture1d()
	{

	}
	Texture1d::~Texture1d()
	{

	}



	// Protected methods:
	void Texture1d::CreateImage(VkImageSubresourceRange& subresourceRange, VkFormat format, VkImageUsageFlags usageFlags, VkImageCreateFlags imageFlags, VkMemoryPropertyFlags memoryFlags, VkImageViewType viewType, const DeviceQueue& queue)
	{
		VkImageType imageType = VK_IMAGE_TYPE_1D;
		CreateImageBase(imageType, subresourceRange, format, usageFlags, imageFlags, memoryFlags, viewType, queue);
	}
}