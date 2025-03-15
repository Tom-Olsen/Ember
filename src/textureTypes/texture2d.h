#ifndef __INCLUDE_GUARD_texture2d_h__
#define __INCLUDE_GUARD_texture2d_h__
#include "vulkanQueue.h"
#include <memory>
#include <string>
#include <filesystem>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	// Forward declarations:
	class VmaBuffer;
	class VmaImage;


	/// <summary>
	/// Polymorphic parent class for different kinds of 2d textures.
	/// Each derivative handles a different type of VkImage.
	/// The images differ in: <para/>
	/// -VkImageUsageFlags		=> transfer, sampling, storage, depth, color. <para/>
	/// -VkImageCreateFlagBits	=> additional specialisations. <para/>
	/// -VkImageViewType		=> single image, array, cube.
	/// </summary>
	class Texture2d
	{
	protected: // Members:
		int m_width;
		int m_height;
		int m_channels;
		std::string m_name;
		std::unique_ptr<VmaImage> m_pImage;
		VkDescriptorType m_descriptorType;

	public: // Methods:
		virtual ~Texture2d();

		// Getters:
		uint64_t GetWidth() const;
		uint64_t GetHeight() const;
		uint64_t GetChannels() const;
		const std::string& GetName() const;
		VmaImage* const GetVmaImage() const;
		VkDescriptorType GetVkDescriptorType() const;

	protected: // Methods:
		uint32_t BytesPerChannel(VkFormat format);
		VmaImage* CreateImage(VkImageSubresourceRange& subresourceRange, VkFormat format, VkImageUsageFlags usageFlags, VkImageCreateFlags imageFlags, VkMemoryPropertyFlags memoryFlags, VkImageViewType viewType, const VulkanQueue& queue);
	};
}



#endif // __INCLUDE_GUARD_texture2d_h__