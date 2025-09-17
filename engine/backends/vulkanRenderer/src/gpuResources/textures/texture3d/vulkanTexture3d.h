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
	/// Polymorphic parent class for different kinds of 3d textures.
	/// Each derivative handles a different type of VkImage.
	/// The images differ in: <para/>
	/// -VkImageUsageFlags		=> transfer, sampling, storage, depth, color. <para/>
	/// -VkImageCreateFlagBits	=> additional specialisations. <para/>
	/// -VkImageViewType		=> single image, array.
	/// </summary>
	class Texture3d : public Texture
	{
	public: // Methods:
		// Constructor/Destructor:
		Texture3d();
		virtual ~Texture3d();

		// Non-copyable:
		Texture3d(const Texture3d&) = delete;
		Texture3d& operator=(const Texture3d&) = delete;

		// Movable:
		Texture3d(Texture3d&& other) noexcept = default;
		Texture3d& operator=(Texture3d&& other) noexcept = default;

	protected: // Methods:
		void CreateImage(VkImageSubresourceRange& subresourceRange, VkFormat format, VkImageUsageFlags usageFlags, VkImageCreateFlags imageFlags, VkMemoryPropertyFlags memoryFlags, VkImageViewType viewType, const DeviceQueue& queue);
	};
}