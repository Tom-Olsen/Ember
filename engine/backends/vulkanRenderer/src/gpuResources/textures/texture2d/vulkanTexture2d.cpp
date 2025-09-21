#include "vulkanTexture2d.h"



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
	void Texture2d::CreateImage(ImageSubresourceRange& subresourceRange, Format format, ImageUsageFlag usageFlags, ImageCreateFlag imageFlags, MemoryPropertyFlag memoryFlags, ImageViewType viewType, const DeviceQueue& queue)
	{
		ImageType imageType = ImageTypes::image_type_2d;
		CreateImageBase(imageType, subresourceRange, format, usageFlags, imageFlags, memoryFlags, viewType, queue);
	}
}