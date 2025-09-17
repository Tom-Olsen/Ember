#pragma once
#include "vulkanTexture.h"
#include <filesystem>



namespace vulkanRendererBackend
{
	// Forward declerations:
	class StagingBuffer;



	/// <summary>
	/// Polymorphic parent class for different kinds of cube textures.
	/// Each derivative handles a different type of VkImage.
	/// The images differ in: <para/>
	/// -VkImageUsageFlags		=> transfer, sampling, storage, depth, color. <para/>
	/// -VkImageCreateFlagBits	=> additional specialisations. <para/>
	/// -VkImageViewType		=> single image, array.
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
		void CreateImage(VkImageSubresourceRange& subresourceRange, VkFormat format, VkImageUsageFlags usageFlags, VkImageCreateFlags imageFlags, VkMemoryPropertyFlags memoryFlags, VkImageViewType viewType, const DeviceQueue& queue);
	};
}