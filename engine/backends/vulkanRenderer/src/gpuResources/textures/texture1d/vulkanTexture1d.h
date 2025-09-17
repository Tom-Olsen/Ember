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
	/// Polymorphic parent class for different kinds of 1d textures.
	/// Each derivative handles a different type of VkImage.
	/// The images differ in: <para/>
	/// -VkImageUsageFlags		=> transfer, sampling, storage, depth, color. <para/>
	/// -VkImageCreateFlagBits	=> additional specialisations. <para/>
	/// -VkImageViewType		=> single image, array.
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
		void CreateImage(VkImageSubresourceRange& subresourceRange, VkFormat format, VkImageUsageFlags usageFlags, VkImageCreateFlags imageFlags, VkMemoryPropertyFlags memoryFlags, VkImageViewType viewType, const DeviceQueue& queue);
	};
}