#ifndef __INCLUDE_GUARD_texture2d_h__
#define __INCLUDE_GUARD_texture2d_h__
#include <memory>
#include <string>
#include <filesystem>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	// Forward declarations:
	struct VulkanContext;
	class VmaBuffer;
	class VmaImage;



	class Texture2d
	{
	protected: // Members:
		int m_width;
		int m_height;
		int m_channels;
		bool m_isStorageImage; // true=>VK_IMAGE_USAGE_STORAGE_BIT, false=>VK_IMAGE_USAGE_SAMPLED_BIT
		std::string m_name;
		std::unique_ptr<VmaImage> m_pImage;
		VulkanContext* m_pContext;

	public: // Methods:
		// Used for wrapping Texture2d around existing VmaImage (e.g VmaImage of shadowRenderPass). Takes ownership of the VmaImage.
		Texture2d(VulkanContext* pContext, VmaImage* pImage, const std::string& name);
		Texture2d(VulkanContext* pContext, const std::filesystem::path& filePath, const std::string& name, VkFormat format, bool isStorageImage = false);
		~Texture2d();

		// Getters:
		uint64_t GetWidth() const;
		uint64_t GetHeight() const;
		uint64_t GetChannels() const;
		bool GetIsStorageImage() const;
		const VmaImage* const GetVmaImage() const;
		const std::string& GetName() const;

		// Static spezialised Constructors:
		static Texture2d* StorageTexture2d(VulkanContext* pContext, int width, int height, VkFormat format, const std::string& name);

	protected: // Methods:
		Texture2d();
		static VmaImage* CreateImage(VulkanContext* pContext, VkImageSubresourceRange* pSubresourceRange, uint32_t width, uint32_t height, VkFormat format, VkImageCreateFlagBits imageFlags, bool isStorageImage);
		void TransitionImageLayout(VkImageSubresourceRange* pSubresourceRange, VmaBuffer& stagingBuffer);
		void TransitionImageLayoutWithMipMapping(VkImageSubresourceRange* pSubresourceRange, VmaBuffer& stagingBuffer);
	};
}



#endif // __INCLUDE_GUARD_texture2d_h__