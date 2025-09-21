#pragma once
#include "vulkanTexture.h"
#include <filesystem>



namespace vulkanRendererBackend
{
	/// <summary>
	/// Polymorphic parent class for different kinds of cube textures.
	/// Each derivative handles a different type of VkImage.
	/// The images differ in: <para/>
	/// -ImageUsageFlag		=> transfer, sampling, storage, depth, color. <para/>
	/// -ImageCreateFlagBit	=> additional specialisations. <para/>
	/// -ImageViewType		=> single image, array.
	/// </summary>
	class TextureCube : public Texture
	{
	public: // Methods:
		// Constructor/Destructor:
		TextureCube();
		virtual ~TextureCube();

		// Non-copyable:
		TextureCube(const TextureCube&) = delete;
		TextureCube& operator=(const TextureCube&) = delete;

		// Movable:
		TextureCube(TextureCube&& other) noexcept = default;
		TextureCube& operator=(TextureCube&& other) noexcept = default;

	protected: // Methods:
		void CreateImage(ImageSubresourceRange& subresourceRange, Format format, ImageUsageFlag usageFlags, ImageCreateFlag imageFlags, MemoryPropertyFlag memoryFlags, ImageViewType viewType, const DeviceQueue& queue);
	};
}