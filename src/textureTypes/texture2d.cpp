// needs to be defined before including stb_image.h, but may not be in the header file!
#define STB_IMAGE_IMPLEMENTATION
#include "texture2d.h"
#include "logger.h"
#include "stb_image.h"
#include "vmaBuffer.h"
#include "vmaImage.h"



namespace emberEngine
{
	// Virtual Destructor:
	Texture2d::~Texture2d()
	{

	}



	// Public methods:
	// Getters:
	Texture2d::Type Texture2d::GetType() const
	{
		return m_type;
	}
	std::string Texture2d::GetTypeName() const
	{
		switch (m_type)
		{
			case Type::cube: return "TextureCube"; break;
			case Type::sample: return "SampleTexture2d"; break;
			case Type::shadow: return "ShadowTexture2d"; break;
			case Type::storage: return "StorageTexture2d"; break;
			default: return "Texture type could not be identified.";
		}
	}
	uint64_t Texture2d::GetWidth() const
	{
		return static_cast<uint64_t>(m_width);
	}
	uint64_t Texture2d::GetHeight() const
	{
		return static_cast<uint64_t>(m_height);
	}
	uint64_t Texture2d::GetChannels() const
	{
		return static_cast<uint64_t>(m_channels);
	}
	const std::string& Texture2d::GetName() const
	{
		return m_name;
	}
	const VmaImage* const Texture2d::GetVmaImage() const
	{
		return m_pImage.get();
	}
	VkDescriptorType Texture2d::GetVkDescriptorType() const
	{
		return m_descriptorType;
	}



	// Protected methods:
	uint32_t Texture2d::BytesPerChannel(VkFormat format)
	{
		switch (format)
		{
			case VK_FORMAT_R8_UNORM: return 1;
			case VK_FORMAT_R8G8B8A8_SRGB: return 1;
			case VK_FORMAT_R8G8B8A8_UNORM: return 1;
			case VK_FORMAT_R32G32B32A32_SFLOAT: return 4;
			default:
				LOG_ERROR("Unsupported VkFormat: {}", (int)format);
				return -1;
		}
	}
	VmaImage* Texture2d::CreateImage(VkImageSubresourceRange& subresourceRange, VkFormat format, VkImageUsageFlags usageFlags, VkImageCreateFlags imageFlags, VkMemoryPropertyFlags memoryFlags, VkImageViewType viewType, const VulkanQueue& queue)
	{
		VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = m_width;
		imageInfo.extent.height = m_height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = subresourceRange.levelCount;
		imageInfo.arrayLayers = subresourceRange.layerCount;
		imageInfo.format = format;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;	// optimal for GPU. use linear for CPU readback.
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usageFlags;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = imageFlags;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		allocInfo.flags = 0;
		allocInfo.requiredFlags = memoryFlags;
		allocInfo.preferredFlags = 0;

		// Always use transfer queue by default and do queue transition later when needed.
		return new VmaImage(imageInfo, allocInfo, subresourceRange, viewType, queue);
	}
}