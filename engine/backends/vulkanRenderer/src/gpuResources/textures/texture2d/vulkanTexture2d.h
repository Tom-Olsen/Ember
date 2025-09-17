#pragma once
#include "textureFormat.h"
#include "vulkanTexture.h"
#include <memory>
#include <string>
#include <unordered_set>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class StagingBuffer;
	class VmaBuffer;
	class VmaImage;
	struct DeviceQueue;



	/// <summary>
	/// Polymorphic parent class for different kinds of 2d textures.
	/// Each derivative handles a different type of VkImage.
	/// The images differ in: <para/>
	/// -VkImageUsageFlags		=> transfer, sampling, storage, depth, color. <para/>
	/// -VkImageCreateFlagBits	=> additional specialisations. <para/>
	/// -VkImageViewType		=> single image, array.
	/// </summary>
	class Texture2d : public Texture
	{
	public: // Methods:
		// Constructor/Destructor:
		Texture2d();
		virtual ~Texture2d();

		// Non-copyable:
		Texture2d(const Texture2d&) = delete;
		Texture2d& operator=(const Texture2d&) = delete;

		// Movable:
		Texture2d(Texture2d&& other) noexcept = default;
		Texture2d& operator=(Texture2d&& other) noexcept = default;

	protected: // Methods:
		void CreateImage(VkImageSubresourceRange& subresourceRange, VkFormat format, VkImageUsageFlags usageFlags, VkImageCreateFlags imageFlags, VkMemoryPropertyFlags memoryFlags, VkImageViewType viewType, const DeviceQueue& queue);
	};
}