#include "vulkanTexture1d.h"



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
	void Texture1d::CreateImage(ImageSubresourceRange& subresourceRange, Format format, ImageUsageFlag usageFlags, ImageCreateFlag imageFlags, MemoryPropertyFlag memoryFlags, ImageViewType viewType, const DeviceQueue& queue)
	{
		ImageType imageType = ImageTypes::image_type_1d;
		CreateImageBase(imageType, subresourceRange, format, usageFlags, imageFlags, memoryFlags, viewType, queue);
	}
}