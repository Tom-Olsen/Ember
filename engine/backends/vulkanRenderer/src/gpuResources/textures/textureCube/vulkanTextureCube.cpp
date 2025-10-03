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
	void TextureCube::CreateImage(ImageSubresourceRange& subresourceRange, Format format, ImageUsageFlag usageFlags, ImageCreateFlag imageFlags, MemoryPropertyFlag memoryFlags, ImageViewType viewType, const DeviceQueue& queue)
	{
		imageFlags |= ImageCreateFlags::cube_compatible_bit;
		ImageType imageType = ImageTypes::image_type_2d;
		CreateImageBase(imageType, subresourceRange, format, usageFlags, imageFlags, memoryFlags, viewType, queue);
	}
}