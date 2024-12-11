#ifndef __INCLUDE_GUARD_texture2d_h__
#define __INCLUDE_GUARD_texture2d_h__
#include <memory>
#include <string>
#include <filesystem>
#include <vulkan/vulkan.h>



struct VulkanContext;
class VmaBuffer;
class VmaImage;



class Texture2d
{
protected: // Members:
	int m_width;
	int m_height;
	int m_channels;
	std::unique_ptr<VmaImage> m_pImage;
	std::string m_name;
	VulkanContext* m_pContext;

public: // Methods:
	Texture2d(VulkanContext* pContext, VmaImage* pImage, const std::string& name);
	Texture2d(VulkanContext* pContext, const std::filesystem::path& filePath, const std::string& name, VkFormat format);
	~Texture2d();

	// Getters:
	uint64_t GetWidth() const;
	uint64_t GetHeight() const;
	uint64_t GetChannels() const;
	const VmaImage* const GetVmaImage() const;
	const std::string& GetName() const;

protected: // Methods:
	Texture2d();
	void CreateImage(VkImageSubresourceRange* pSubresourceRange, uint32_t width, uint32_t height, VkFormat format, VkImageCreateFlagBits imageFlags);
	void TransitionImageLayout(VkImageSubresourceRange* pSubresourceRange, VmaBuffer& stagingBuffer);
	void TransitionImageLayoutWithMipMapping(VkImageSubresourceRange* pSubresourceRange, VmaBuffer& stagingBuffer);
};



#endif // __INCLUDE_GUARD_texture2d_h__