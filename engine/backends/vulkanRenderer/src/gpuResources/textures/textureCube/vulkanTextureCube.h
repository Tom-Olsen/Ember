#pragma once
#include "vulkanTexture.h"
#include "vulkanRendererExport.h"



namespace vulkanRendererBackend
{
	/// <summary>
	/// Polymorphic parent class for different kinds of cube textures.
	/// Each derivative handles a different type of VkImage.
	/// The images differ in: <para/>
	/// -VkImageUsageFlags	= transfer, sampling, storage, depth, color. <para/>
	/// -VkImageCreateFlags	= additional specialisations. <para/>
	/// -VkImageViewType    = single image, array.
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
		void CreateImage(VkImageSubresourceRange& subresourceRange, Format format, VkImageUsageFlags usageFlags, VkImageCreateFlags imageFlags, VkMemoryPropertyFlags memoryFlags, VkImageViewType viewType, const DeviceQueue& queue);
	};
}