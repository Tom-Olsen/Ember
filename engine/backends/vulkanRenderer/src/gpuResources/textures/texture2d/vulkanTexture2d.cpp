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
	void Texture2d::CreateImage(ImageSubresourceRange& subresourceRange, Format format, ImageUsageFlag usageFlags, ImageCreateFlag imageFlags, MemoryPropertyFlag memoryFlags, ImageViewType viewType, const DeviceQueue& queue)
	{
		ImageType imageType = ImageTypes::image_type_2d;
		CreateImageBase(imageType, subresourceRange, format, usageFlags, imageFlags, memoryFlags, viewType, queue);
	}
}