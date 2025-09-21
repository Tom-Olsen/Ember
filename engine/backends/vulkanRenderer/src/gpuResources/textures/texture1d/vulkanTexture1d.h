#pragma once
#include "vulkanTexture.h"



namespace vulkanRendererBackend
{
	/// <summary>
	/// Polymorphic parent class for different kinds of 1d textures.
	/// Each derivative handles a different type of VkImage.
	/// The images differ in: <para/>
	/// -ImageUsageFlag		=> transfer, sampling, storage, depth, color. <para/>
	/// -ImageCreateFlagBit	=> additional specialisations. <para/>
	/// -ImageViewType		=> single image, array.
	/// </summary>
	class Texture1d : public Texture
	{
	public: // Methods:
		// Constructor/Destructor:
		Texture1d();
		virtual ~Texture1d();

		// Non-copyable:
		Texture1d(const Texture1d&) = delete;
		Texture1d& operator=(const Texture1d&) = delete;

		// Movable:
		Texture1d(Texture1d&& other) noexcept = default;
		Texture1d& operator=(Texture1d&& other) noexcept = default;

	protected: // Methods:
		void CreateImage(ImageSubresourceRange& subresourceRange, Format format, ImageUsageFlag usageFlags, ImageCreateFlag imageFlags, MemoryPropertyFlag memoryFlags, ImageViewType viewType, const DeviceQueue& queue);
	};
}