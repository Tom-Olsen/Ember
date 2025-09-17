#include "vulkanTexture3d.h"
#include "vmaImage.h"
#include "vulkanDeviceQueue.h"



namespace vulkanRendererBackend
{
	// Virtual Destructor:
	Texture3d::Texture3d()
	{

	}
	Texture3d::~Texture3d()
	{

	}



	// Protected methods:
	void Texture3d::CreateImage(VkImageSubresourceRange& subresourceRange, VkFormat format, VkImageUsageFlags usageFlags, VkImageCreateFlags imageFlags, VkMemoryPropertyFlags memoryFlags, VkImageViewType viewType, const DeviceQueue& queue)
	{
		VkImageType imageType = VK_IMAGE_TYPE_3D;
		CreateImageBase(imageType, subresourceRange, format, usageFlags, imageFlags, memoryFlags, viewType, queue);
	}
}