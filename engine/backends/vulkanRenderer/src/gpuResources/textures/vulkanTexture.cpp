#include "vulkanTexture.h"
#include "vmaImage.h"
#include "vulkanDeviceQueue.h"
#include "vulkanConvertTextureFormat.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Static members:
	std::unordered_set<Format> Texture::s_valid08BitFormats =
	{
		Formats::r8_srgb, Formats::r8_uint, Formats::r8_sint, Formats::r8_uscaled, Formats::r8_sscaled, Formats::r8_unorm, Formats::r8_snorm,
		Formats::r8g8_srgb, Formats::r8g8_uint, Formats::r8g8_sint, Formats::r8g8_uscaled, Formats::r8g8_sscaled, Formats::r8g8_unorm, Formats::r8g8_snorm,
		Formats::r8g8b8_srgb, Formats::r8g8b8_uint, Formats::r8g8b8_sint, Formats::r8g8b8_uscaled, Formats::r8g8b8_sscaled, Formats::r8g8b8_unorm, Formats::r8g8b8_snorm,
		Formats::r8g8b8a8_srgb, Formats::r8g8b8a8_uint, Formats::r8g8b8a8_sint, Formats::r8g8b8a8_uscaled, Formats::r8g8b8a8_sscaled, Formats::r8g8b8a8_unorm, Formats::r8g8b8a8_snorm,
	};
	std::unordered_set<Format> Texture::s_valid16BitFormats =
	{
		Formats::r16_uint, Formats::r16_sint, Formats::r16_uscaled, Formats::r16_sscaled, Formats::r16_unorm, Formats::r16_snorm, Formats::r16_sfloat,
		Formats::r16g16_uint, Formats::r16g16_sint, Formats::r16g16_uscaled, Formats::r16g16_sscaled, Formats::r16g16_unorm, Formats::r16g16_snorm, Formats::r16g16_sfloat,
		Formats::r16g16b16_uint, Formats::r16g16b16_sint, Formats::r16g16b16_uscaled, Formats::r16g16b16_sscaled, Formats::r16g16b16_unorm, Formats::r16g16b16_snorm, Formats::r16g16b16_sfloat,
		Formats::r16g16b16a16_uint, Formats::r16g16b16a16_sint, Formats::r16g16b16a16_uscaled, Formats::r16g16b16a16_sscaled, Formats::r16g16b16a16_unorm, Formats::r16g16b16a16_snorm, Formats::r16g16b16a16_sfloat,
	};
	std::unordered_set<Format> Texture::s_valid32BitFormats =
	{
		Formats::r32_uint, Formats::r32_sint, Formats::r32_sfloat,
		Formats::r32g32_uint, Formats::r32g32_sint, Formats::r32g32_sfloat,
		Formats::r32g32b32_uint, Formats::r32g32b32_sint, Formats::r32g32b32_sfloat,
		Formats::r32g32b32a32_uint, Formats::r32g32b32a32_sint, Formats::r32g32b32a32_sfloat,
	};
	std::unordered_set<Format> Texture::s_valid64BitFormats =
	{
		Formats::r64_uint, Formats::r64_sint, Formats::r64_sfloat,
		Formats::r64g64_uint, Formats::r64g64_sint, Formats::r64g64_sfloat,
		Formats::r64g64b64_uint, Formats::r64g64b64_sint, Formats::r64g64b64_sfloat,
		Formats::r64g64b64a64_uint, Formats::r64g64b64a64_sint, Formats::r64g64b64a64_sfloat
	};
	std::unordered_set<Format> Texture::s_validSingleChannelFormats =
	{
		Formats::r8_srgb, Formats::r8_uint, Formats::r8_sint, Formats::r8_uscaled, Formats::r8_sscaled, Formats::r8_unorm, Formats::r8_snorm,
		Formats::r16_uint, Formats::r16_sint, Formats::r16_uscaled, Formats::r16_sscaled, Formats::r16_unorm, Formats::r16_snorm, Formats::r16_sfloat,
		Formats::r32_uint, Formats::r32_sint, Formats::r32_sfloat,
		Formats::r64_uint, Formats::r64_sint, Formats::r64_sfloat,
	};
	std::unordered_set<Format> Texture::s_validDoubleChannelFormats =
	{
		Formats::r8g8_srgb, Formats::r8g8_uint, Formats::r8g8_sint, Formats::r8g8_uscaled, Formats::r8g8_sscaled, Formats::r8g8_unorm, Formats::r8g8_snorm,
		Formats::r16g16_uint, Formats::r16g16_sint, Formats::r16g16_uscaled, Formats::r16g16_sscaled, Formats::r16g16_unorm, Formats::r16g16_snorm, Formats::r16g16_sfloat,
		Formats::r32g32_uint, Formats::r32g32_sint, Formats::r32g32_sfloat,
		Formats::r64g64_uint, Formats::r64g64_sint, Formats::r64g64_sfloat,
	};
	std::unordered_set<Format> Texture::s_validTripleChannelFormats =
	{
		Formats::r8g8b8_srgb, Formats::r8g8b8_uint, Formats::r8g8b8_sint, Formats::r8g8b8_uscaled, Formats::r8g8b8_sscaled, Formats::r8g8b8_unorm, Formats::r8g8b8_snorm,
		Formats::r16g16b16_uint, Formats::r16g16b16_sint, Formats::r16g16b16_uscaled, Formats::r16g16b16_sscaled, Formats::r16g16b16_unorm, Formats::r16g16b16_snorm, Formats::r16g16b16_sfloat,
		Formats::r32g32b32_uint, Formats::r32g32b32_sint, Formats::r32g32b32_sfloat,
		Formats::r64g64b64_uint, Formats::r64g64b64_sint, Formats::r64g64b64_sfloat,
	};
	std::unordered_set<Format> Texture::s_validQuadrupleChannelFormats =
	{
		Formats::r8g8b8a8_srgb, Formats::r8g8b8a8_uint, Formats::r8g8b8a8_sint, Formats::r8g8b8a8_uscaled, Formats::r8g8b8a8_sscaled, Formats::r8g8b8a8_unorm, Formats::r8g8b8a8_snorm,
		Formats::r16g16b16a16_uint, Formats::r16g16b16a16_sint, Formats::r16g16b16a16_uscaled, Formats::r16g16b16a16_sscaled, Formats::r16g16b16a16_unorm, Formats::r16g16b16a16_snorm, Formats::r16g16b16a16_sfloat,
		Formats::r32g32b32a32_uint, Formats::r32g32b32a32_sint, Formats::r32g32b32a32_sfloat,
		Formats::r64g64b64a64_uint, Formats::r64g64b64a64_sint, Formats::r64g64b64a64_sfloat
	};
	std::unordered_set<Format> Texture::s_validDepthFormats =
	{ Formats::d16_unorm, Formats::d16_unorm_s8_uint, Formats::d24_unorm_s8_uint, Formats::d32_sfloat, Formats::d32_sfloat_s8_uint };
	std::unordered_set<Format> Texture::s_validStencilFormats =
	{ Formats::s8_uint, Formats::d16_unorm_s8_uint, Formats::d24_unorm_s8_uint, Formats::d32_sfloat_s8_uint };



	// Protected methods:
	// Constructor:
	Texture::Texture()
	{

	}



	// Public methods:
	// Destructor:
	Texture::~Texture()
	{

	}



	// Movable:
	Texture::Texture(Texture&& other) noexcept = default;
	Texture& Texture::operator=(Texture&& other) noexcept = default;



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
	const emberCommon::TextureFormat& Texture::GetTextureFormat() const
	{
		return VulkanFormatToTextureFormat(m_format);
	}
	void* Texture::GetVkImageView() const
	{
		return static_cast<void*>(GetVmaImage()->GetVkImageView());
	}
	Format Texture::GetFormat() const
	{
		return m_format;
	}
	VmaImage* const Texture::GetVmaImage() const
	{
		return m_pImage.get();
	}
	DescriptorType Texture::GetDescriptorType() const
	{
		return m_descriptorType;
	}



	// Protected methods:
	uint32_t Texture::GetChannelCount(Format format)
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

		throw std::runtime_error("Unsupported Texture format: " + Formats::ToString(format));
		return -1;
	}
	uint32_t Texture::BytesPerChannel(Format format)
	{
		if (s_valid08BitFormats.contains(format))
			return 1;
		if (s_valid16BitFormats.contains(format))
			return 2;
		if (s_valid32BitFormats.contains(format))
			return 4;
		if (s_valid64BitFormats.contains(format))
			return 8;

		throw std::runtime_error("Unsupported Texture format: " + Formats::ToString(format));
		return -1;
	}
	bool Texture::IsValidImageFormat(Format format)
	{
		return s_valid08BitFormats.contains(format) || s_valid16BitFormats.contains(format) || s_valid32BitFormats.contains(format) || s_valid64BitFormats.contains(format);
	}
	bool Texture::IsDepthFormat(Format format)
	{
		return s_validDepthFormats.contains(format);
	}
	bool Texture::IsStencilFormat(Format format)
	{
		return s_validStencilFormats.contains(format);
	}
	void Texture::CreateImageBase(ImageType imageType, ImageSubresourceRange& subresourceRange, Format format, ImageUsageFlag usageFlags, ImageCreateFlag imageFlags, MemoryPropertyFlag memoryFlags, ImageViewType viewType, const DeviceQueue& queue)
	{
		ImageCreateInfo imageInfo = {};
		imageInfo.imageType = imageType;
		imageInfo.extent.x = m_width;
		imageInfo.extent.y = m_height;
		imageInfo.extent.z = m_depth;
		imageInfo.mipLevels = subresourceRange.levelCount;
		imageInfo.arrayLayers = subresourceRange.layerCount;
		imageInfo.format = format;
		imageInfo.tiling = ImageTilings::optimal;
		imageInfo.initialLayout = ImageLayouts::undefined;
		imageInfo.usages = usageFlags;
		imageInfo.sharingMode = SharingModes::exclusive;
		imageInfo.sampleCountFlags = SampleCountFlags::sample_count_1_bit;
		imageInfo.flags = imageFlags;
		imageInfo.queueFamilyIndexCount = 1;
		imageInfo.pQueueFamilyIndices = &queue.familyIndex;

		AllocationCreateInfo allocInfo = {};
		allocInfo.usages = MemoryUsages::auto_prefer_device;
		allocInfo.requiredFlags = memoryFlags;

		m_pImage = std::make_unique<VmaImage>(m_name, imageInfo, allocInfo, subresourceRange, viewType, queue);
	}
}