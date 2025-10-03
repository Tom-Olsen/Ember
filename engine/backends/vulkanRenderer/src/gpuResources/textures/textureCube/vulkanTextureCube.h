#pragma once
#include "vulkanTexture.h"
#include "vulkanRendererExport.h"



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
	class VULKAN_RENDERER_API TextureCube : public Texture
	{
	protected: // Methods:
		// Constructor:
		TextureCube();

	public: // Methods:
		// Destructor:
		virtual ~TextureCube();

		// Non-copyable:
		TextureCube(const TextureCube&) = delete;
		TextureCube& operator=(const TextureCube&) = delete;

		// Movable:
		TextureCube(TextureCube&& other) noexcept;
		TextureCube& operator=(TextureCube&& other) noexcept;

	protected: // Methods:
		void CreateImage(ImageSubresourceRange& subresourceRange, Format format, ImageUsageFlag usageFlags, ImageCreateFlag imageFlags, MemoryPropertyFlag memoryFlags, ImageViewType viewType, const DeviceQueue& queue);
	};
}