#include "vulkanTexture3d.h"



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
	void Texture3d::CreateImage(ImageSubresourceRange& subresourceRange, Format format, ImageUsageFlag usageFlags, ImageCreateFlag imageFlags, MemoryPropertyFlag memoryFlags, ImageViewType viewType, const DeviceQueue& queue)
	{
		ImageType imageType = ImageTypes::image_type_3d;
		CreateImageBase(imageType, subresourceRange, format, usageFlags, imageFlags, memoryFlags, viewType, queue);
	}
}