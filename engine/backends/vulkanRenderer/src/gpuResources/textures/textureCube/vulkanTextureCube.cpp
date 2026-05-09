#include "vulkanTextureCube.h"



namespace vulkanRendererBackend
{
	// Protected method:
	// Constructor:
    TextureCube::TextureCube()
	{
		m_depth = 1;
	}



	// Public method:
	// Desctructor:
	TextureCube::~TextureCube()
	{

	}

	// Movable:
	TextureCube::TextureCube(TextureCube&& other) noexcept = default;
	TextureCube& TextureCube::operator=(TextureCube&& other) noexcept = default;



	// Protected methods:
	void TextureCube::CreateImage(VkImageSubresourceRange& subresourceRange, Format format, VkImageUsageFlags usageFlags, VkImageCreateFlags imageFlags, MemoryPropertyFlag memoryFlags, VkImageViewType viewType, const DeviceQueue& queue)
	{
		imageFlags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		VkImageType imageType = VK_IMAGE_TYPE_2D;
		CreateImageBase(imageType, subresourceRange, format, usageFlags, imageFlags, memoryFlags, viewType, queue);
	}
}