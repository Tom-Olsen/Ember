#include "vulkanTexture2d.h"



namespace vulkanRendererBackend
{
	// Protected methods:
	// Constructor:
	Texture2d::Texture2d()
	{
		m_depth = 1;
	}



	// Public methods:
	// Destructor:
	Texture2d::~Texture2d()
	{

	}



	// Movable:
	Texture2d::Texture2d(Texture2d&& other) noexcept = default;
	Texture2d& Texture2d::operator=(Texture2d&& other) noexcept = default;



	// Protected methods:
	void Texture2d::CreateImage(VkImageSubresourceRange& subresourceRange, VkFormat format, VkImageUsageFlags usageFlags, VkImageCreateFlags imageFlags, VkMemoryPropertyFlags memoryFlags, VkImageViewType viewType, const DeviceQueue& queue)
	{
		VkImageType imageType = VK_IMAGE_TYPE_2D;
		CreateImageBase(imageType, subresourceRange, format, usageFlags, imageFlags, memoryFlags, viewType, queue);
	}
}