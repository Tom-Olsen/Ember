#include "vulkanTextureCube.h"



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
	void TextureCube::CreateImage(ImageSubresourceRange& subresourceRange, Format format, ImageUsageFlag usageFlags, ImageCreateFlag imageFlags, MemoryPropertyFlag memoryFlags, ImageViewType viewType, const DeviceQueue& queue)
	{
		imageFlags |= ImageCreateFlags::cube_compatible_bit;
		ImageType imageType = ImageTypes::image_type_2d;
		CreateImageBase(imageType, subresourceRange, format, usageFlags, imageFlags, memoryFlags, viewType, queue);
	}
}