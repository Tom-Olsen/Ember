#pragma once
#include "vulkanTexture.h"
#include "vulkanRendererExport.h"



namespace vulkanRendererBackend
{
	// Forward declarations:
	class StagingBuffer;
	class VmaBuffer;
	class VmaImage;
	struct DeviceQueue;



	/// <summary>
	/// Polymorphic parent class for different kinds of 3d textures.
	/// Each derivative handles a different type of VkImage.
	/// The images differ in: <para/>
	/// -ImageUsageFlag		=> transfer, sampling, storage, depth, color. <para/>
	/// -ImageCreateFlagBit	=> additional specialisations. <para/>
	/// -ImageViewType		=> single image, array.
	/// </summary>
	class VULKAN_RENDERER_API Texture3d : public Texture
	{
	protected: // Methods:
		// Constructor:
		Texture3d();

	public: // Methods:
		// Destructor:
		virtual ~Texture3d();

		// Non-copyable:
		Texture3d(const Texture3d&) = delete;
		Texture3d& operator=(const Texture3d&) = delete;

		// Movable:
		Texture3d(Texture3d&& other) noexcept;
		Texture3d& operator=(Texture3d&& other) noexcept;

	protected: // Methods:
		void CreateImage(ImageSubresourceRange& subresourceRange, Format format, ImageUsageFlag usageFlags, ImageCreateFlag imageFlags, MemoryPropertyFlag memoryFlags, ImageViewType viewType, const DeviceQueue& queue);
	};
}