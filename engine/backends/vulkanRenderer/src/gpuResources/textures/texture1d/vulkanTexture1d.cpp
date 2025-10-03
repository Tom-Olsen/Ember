#include "vulkanTexture1d.h"



namespace vulkanRendererBackend
{
	// Protected methods:
	// Constructor:
	Texture1d::Texture1d()
	{
		m_height = 1;
		m_depth = 1;
	}



	// Public methods:
	// Destructor:
	Texture1d::~Texture1d()
	{

	}



	// Movable:
	Texture1d::Texture1d(Texture1d&& other) noexcept = default;
	Texture1d& Texture1d::operator=(Texture1d&& other) noexcept = default;



	// Protected methods:
	void Texture1d::CreateImage(ImageSubresourceRange& subresourceRange, Format format, ImageUsageFlag usageFlags, ImageCreateFlag imageFlags, MemoryPropertyFlag memoryFlags, ImageViewType viewType, const DeviceQueue& queue)
	{
		ImageType imageType = ImageTypes::image_type_1d;
		CreateImageBase(imageType, subresourceRange, format, usageFlags, imageFlags, memoryFlags, viewType, queue);
	}
}