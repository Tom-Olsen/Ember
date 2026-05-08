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
	void Texture3d::CreateImage(ImageSubresourceRange& subresourceRange, Format format, ImageUsageFlag usageFlags, ImageCreateFlag imageFlags, MemoryPropertyFlag memoryFlags, VkImageViewType viewType, const DeviceQueue& queue)
	{
		VkImageType imageType = VK_IMAGE_TYPE_3D;
		CreateImageBase(imageType, subresourceRange, format, usageFlags, imageFlags, memoryFlags, viewType, queue);
	}
}