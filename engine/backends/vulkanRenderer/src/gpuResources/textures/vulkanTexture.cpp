#include "vulkanTexture.h"
#include "logger.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanDeviceQueue.h"
#include "vulkanTextureFormatTranslation.h"



namespace vulkanRendererBackend
{
	// Static members:
	std::unordered_set<VkFormat> Texture::s_valid08BitFormats =
	{
		VK_FORMAT_R8_SRGB, VK_FORMAT_R8_UINT, VK_FORMAT_R8_SINT, VK_FORMAT_R8_USCALED, VK_FORMAT_R8_SSCALED, VK_FORMAT_R8_UNORM, VK_FORMAT_R8_SNORM,
		VK_FORMAT_R8G8_SRGB, VK_FORMAT_R8G8_UINT, VK_FORMAT_R8G8_SINT, VK_FORMAT_R8G8_USCALED, VK_FORMAT_R8G8_SSCALED, VK_FORMAT_R8G8_UNORM, VK_FORMAT_R8G8_SNORM,
		VK_FORMAT_R8G8B8_SRGB, VK_FORMAT_R8G8B8_UINT, VK_FORMAT_R8G8B8_SINT, VK_FORMAT_R8G8B8_USCALED, VK_FORMAT_R8G8B8_SSCALED, VK_FORMAT_R8G8B8_UNORM, VK_FORMAT_R8G8B8_SNORM,
		VK_FORMAT_R8G8B8A8_SRGB, VK_FORMAT_R8G8B8A8_UINT, VK_FORMAT_R8G8B8A8_SINT, VK_FORMAT_R8G8B8A8_USCALED, VK_FORMAT_R8G8B8A8_SSCALED, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R8G8B8A8_SNORM,
	};
	std::unordered_set<VkFormat> Texture::s_valid16BitFormats =
	{
		VK_FORMAT_R16_UINT, VK_FORMAT_R16_SINT, VK_FORMAT_R16_USCALED, VK_FORMAT_R16_SSCALED, VK_FORMAT_R16_UNORM, VK_FORMAT_R16_SNORM, VK_FORMAT_R16_SFLOAT,
		VK_FORMAT_R16G16_UINT, VK_FORMAT_R16G16_SINT, VK_FORMAT_R16G16_USCALED, VK_FORMAT_R16G16_SSCALED, VK_FORMAT_R16G16_UNORM, VK_FORMAT_R16G16_SNORM, VK_FORMAT_R16G16_SFLOAT,
		VK_FORMAT_R16G16B16_UINT, VK_FORMAT_R16G16B16_SINT, VK_FORMAT_R16G16B16_USCALED, VK_FORMAT_R16G16B16_SSCALED, VK_FORMAT_R16G16B16_UNORM, VK_FORMAT_R16G16B16_SNORM, VK_FORMAT_R16G16B16_SFLOAT,
		VK_FORMAT_R16G16B16A16_UINT, VK_FORMAT_R16G16B16A16_SINT, VK_FORMAT_R16G16B16A16_USCALED, VK_FORMAT_R16G16B16A16_SSCALED, VK_FORMAT_R16G16B16A16_UNORM, VK_FORMAT_R16G16B16A16_SNORM, VK_FORMAT_R16G16B16A16_SFLOAT,
	};
	std::unordered_set<VkFormat> Texture::s_valid32BitFormats =
	{
		VK_FORMAT_R32_UINT, VK_FORMAT_R32_SINT, VK_FORMAT_R32_SFLOAT,
		VK_FORMAT_R32G32_UINT, VK_FORMAT_R32G32_SINT, VK_FORMAT_R32G32_SFLOAT,
		VK_FORMAT_R32G32B32_UINT, VK_FORMAT_R32G32B32_SINT, VK_FORMAT_R32G32B32_SFLOAT,
		VK_FORMAT_R32G32B32A32_UINT, VK_FORMAT_R32G32B32A32_SINT, VK_FORMAT_R32G32B32A32_SFLOAT,
	};
	std::unordered_set<VkFormat> Texture::s_valid64BitFormats =
	{
		VK_FORMAT_R64_UINT, VK_FORMAT_R64_SINT, VK_FORMAT_R64_SFLOAT,
		VK_FORMAT_R64G64_UINT, VK_FORMAT_R64G64_SINT, VK_FORMAT_R64G64_SFLOAT,
		VK_FORMAT_R64G64B64_UINT, VK_FORMAT_R64G64B64_SINT, VK_FORMAT_R64G64B64_SFLOAT,
		VK_FORMAT_R64G64B64A64_UINT, VK_FORMAT_R64G64B64A64_SINT, VK_FORMAT_R64G64B64A64_SFLOAT
	};
	std::unordered_set<VkFormat> Texture::s_validSingleChannelFormats =
	{
		VK_FORMAT_R8_SRGB, VK_FORMAT_R8_UINT, VK_FORMAT_R8_SINT, VK_FORMAT_R8_USCALED, VK_FORMAT_R8_SSCALED, VK_FORMAT_R8_UNORM, VK_FORMAT_R8_SNORM,
		VK_FORMAT_R16_UINT, VK_FORMAT_R16_SINT, VK_FORMAT_R16_USCALED, VK_FORMAT_R16_SSCALED, VK_FORMAT_R16_UNORM, VK_FORMAT_R16_SNORM, VK_FORMAT_R16_SFLOAT,
		VK_FORMAT_R32_UINT, VK_FORMAT_R32_SINT, VK_FORMAT_R32_SFLOAT,
		VK_FORMAT_R64_UINT, VK_FORMAT_R64_SINT, VK_FORMAT_R64_SFLOAT,
	};
	std::unordered_set<VkFormat> Texture::s_validDoubleChannelFormats =
	{
		VK_FORMAT_R8G8_SRGB, VK_FORMAT_R8G8_UINT, VK_FORMAT_R8G8_SINT, VK_FORMAT_R8G8_USCALED, VK_FORMAT_R8G8_SSCALED, VK_FORMAT_R8G8_UNORM, VK_FORMAT_R8G8_SNORM,
		VK_FORMAT_R16G16_UINT, VK_FORMAT_R16G16_SINT, VK_FORMAT_R16G16_USCALED, VK_FORMAT_R16G16_SSCALED, VK_FORMAT_R16G16_UNORM, VK_FORMAT_R16G16_SNORM, VK_FORMAT_R16G16_SFLOAT,
		VK_FORMAT_R32G32_UINT, VK_FORMAT_R32G32_SINT, VK_FORMAT_R32G32_SFLOAT,
		VK_FORMAT_R64G64_UINT, VK_FORMAT_R64G64_SINT, VK_FORMAT_R64G64_SFLOAT,
	};
	std::unordered_set<VkFormat> Texture::s_validTripleChannelFormats =
	{
		VK_FORMAT_R8G8B8_SRGB, VK_FORMAT_R8G8B8_UINT, VK_FORMAT_R8G8B8_SINT, VK_FORMAT_R8G8B8_USCALED, VK_FORMAT_R8G8B8_SSCALED, VK_FORMAT_R8G8B8_UNORM, VK_FORMAT_R8G8B8_SNORM,
		VK_FORMAT_R16G16B16_UINT, VK_FORMAT_R16G16B16_SINT, VK_FORMAT_R16G16B16_USCALED, VK_FORMAT_R16G16B16_SSCALED, VK_FORMAT_R16G16B16_UNORM, VK_FORMAT_R16G16B16_SNORM, VK_FORMAT_R16G16B16_SFLOAT,
		VK_FORMAT_R32G32B32_UINT, VK_FORMAT_R32G32B32_SINT, VK_FORMAT_R32G32B32_SFLOAT,
		VK_FORMAT_R64G64B64_UINT, VK_FORMAT_R64G64B64_SINT, VK_FORMAT_R64G64B64_SFLOAT,
	};
	std::unordered_set<VkFormat> Texture::s_validQuadrupleChannelFormats =
	{
		VK_FORMAT_R8G8B8A8_SRGB, VK_FORMAT_R8G8B8A8_UINT, VK_FORMAT_R8G8B8A8_SINT, VK_FORMAT_R8G8B8A8_USCALED, VK_FORMAT_R8G8B8A8_SSCALED, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R8G8B8A8_SNORM,
		VK_FORMAT_R16G16B16A16_UINT, VK_FORMAT_R16G16B16A16_SINT, VK_FORMAT_R16G16B16A16_USCALED, VK_FORMAT_R16G16B16A16_SSCALED, VK_FORMAT_R16G16B16A16_UNORM, VK_FORMAT_R16G16B16A16_SNORM, VK_FORMAT_R16G16B16A16_SFLOAT,
		VK_FORMAT_R32G32B32A32_UINT, VK_FORMAT_R32G32B32A32_SINT, VK_FORMAT_R32G32B32A32_SFLOAT,
		VK_FORMAT_R64G64B64A64_UINT, VK_FORMAT_R64G64B64A64_SINT, VK_FORMAT_R64G64B64A64_SFLOAT
	};
	std::unordered_set<VkFormat> Texture::s_validDepthFormats =
	{ VK_FORMAT_D16_UNORM, VK_FORMAT_D16_UNORM_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT };
	std::unordered_set<VkFormat> Texture::s_validStencilFormats =
	{ VK_FORMAT_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT_S8_UINT };



	// Virtual Destructor:
	Texture::~Texture()
	{

	}



	// Public methods:
	// Getters:
	const std::string& Texture::GetName() const
	{
		return m_name;
	}
	uint32_t Texture::GetWidth() const
	{
		return m_width;
	}
	uint32_t Texture::GetHeight() const
	{
		return m_height;
	}
	uint32_t Texture::GetDepth() const
	{
		return m_depth;
	}
	uint32_t Texture::GetChannels() const
	{
		return m_channels;
	}
	const emberEngine::TextureFormat& Texture::GetFormat() const
	{
		return TranslateTextureFormat(m_format);
	}
	VkFormat Texture::GetVkFormat() const
	{
		return m_format;
	}
	VmaImage* const Texture::GetVmaImage() const
	{
		return m_pImage.get();
	}
	VkDescriptorType Texture::GetVkDescriptorType() const
	{
		return m_descriptorType;
	}



	// Protected methods:
	uint32_t Texture::GetChannelCount(VkFormat format)
	{
		if (s_validSingleChannelFormats.contains(format))
			return 1;
		if (s_validDoubleChannelFormats.contains(format))
			return 2;
		if (s_validTripleChannelFormats.contains(format))
			return 3;
		if (s_validQuadrupleChannelFormats.contains(format))
			return 4;

		int channels = 0;
		if (s_validDepthFormats.contains(format))
			channels += 1;
		if (s_validStencilFormats.contains(format))
			channels += 1;
		if (channels > 0)
			return channels;

		throw std::runtime_error("Unsupported Texture format: " + std::to_string(static_cast<int>(format)));
		return -1;
	}
	uint32_t Texture::BytesPerChannel(VkFormat format)
	{
		if (s_valid08BitFormats.contains(format))
			return 1;
		if (s_valid16BitFormats.contains(format))
			return 2;
		if (s_valid32BitFormats.contains(format))
			return 4;
		if (s_valid64BitFormats.contains(format))
			return 8;

		throw std::runtime_error("Unsupported Texture format: " + std::to_string(static_cast<int>(format)));
		return -1;
	}
	bool Texture::IsValidImageFormat(VkFormat format)
	{
		return s_valid08BitFormats.contains(format) || s_valid16BitFormats.contains(format) || s_valid32BitFormats.contains(format) || s_valid64BitFormats.contains(format);
	}
	bool Texture::IsDepthFormat(VkFormat format)
	{
		return s_validDepthFormats.contains(format);
	}
	bool Texture::IsStencilFormat(VkFormat format)
	{
		return s_validStencilFormats.contains(format);
	}
	void Texture::CreateImageBase(VkImageType imageType, VkImageSubresourceRange& subresourceRange, VkFormat format, VkImageUsageFlags usageFlags, VkImageCreateFlags imageFlags, VkMemoryPropertyFlags memoryFlags, VkImageViewType viewType, const DeviceQueue& queue)
	{
		VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		imageInfo.imageType = imageType;
		imageInfo.extent.width = m_width;
		imageInfo.extent.height = m_height;
		imageInfo.extent.depth = m_depth;
		imageInfo.mipLevels = subresourceRange.levelCount;
		imageInfo.arrayLayers = subresourceRange.layerCount;
		imageInfo.format = format;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usageFlags;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = imageFlags;
		imageInfo.queueFamilyIndexCount = 1;
		imageInfo.pQueueFamilyIndices = &queue.familyIndex;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		allocInfo.requiredFlags = memoryFlags;

		m_pImage = std::make_unique<VmaImage>(m_name, imageInfo, allocInfo, subresourceRange, viewType, queue);
	}
}