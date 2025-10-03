#include "vulkanTexture3d.h"



namespace vulkanRendererBackend
{
	// Protected method:
	// Constructor:
	Texture3d::Texture3d()
	{

	}



	// Public method:
	// Desctructor:
	Texture3d::~Texture3d()
	{

	}



	// Movable:
	Texture3d::Texture3d(Texture3d&& other) noexcept = default;
	Texture3d& Texture3d::operator=(Texture3d&& other) noexcept = default;



	// Protected methods:
	void Texture3d::CreateImage(ImageSubresourceRange& subresourceRange, Format format, ImageUsageFlag usageFlags, ImageCreateFlag imageFlags, MemoryPropertyFlag memoryFlags, ImageViewType viewType, const DeviceQueue& queue)
	{
		ImageType imageType = ImageTypes::image_type_3d;
		CreateImageBase(imageType, subresourceRange, format, usageFlags, imageFlags, memoryFlags, viewType, queue);
	}
}