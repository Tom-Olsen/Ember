#pragma once
#include "vulkanTexture.h"
#include "vulkanRendererExport.h"



namespace vulkanRendererBackend
{
	// Forward declarations:
	class StagingBuffer;
	class VmaBuffer;
	class VmaImage;



	/// <summary>
	/// Polymorphic parent class for different kinds of 2d textures.
	/// Each derivative handles a different type of VkImage.
	/// The images differ in: <para/>
	/// -VkImageUsageFlags	= transfer, sampling, storage, depth, color. <para/>
	/// -VkImageCreateFlags	= additional specialisations. <para/>
	/// -VkImageViewType	= single image, array.
	/// </summary>
	class VULKAN_RENDERER_API Texture2d : public Texture
	{
	protected: // Methods:
		// Constructor:
		Texture2d();

	public: // Methods:
		// Destructor:
		virtual ~Texture2d();

		// Non-copyable:
		Texture2d(const Texture2d&) = delete;
		Texture2d& operator=(const Texture2d&) = delete;

		// Movable:
		Texture2d(Texture2d&& other) noexcept;
		Texture2d& operator=(Texture2d&& other) noexcept;

	protected: // Methods:
		void CreateImage(VkImageSubresourceRange& subresourceRange, Format format, VkImageUsageFlags usageFlags, VkImageCreateFlags imageFlags, MemoryPropertyFlag memoryFlags, VkImageViewType viewType, const DeviceQueue& queue);
	};
}