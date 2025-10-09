#pragma once
#include "iTexture.h"
#include "commonTextureFormat.h"
#include "vulkanFormat.h"
#include "vulkanDescriptorType.h"
#include "vulkanImageCreateFlag.h"
#include "vulkanImageSubresourceRange.h"
#include "vulkanImageUsageFlag.h"
#include "vulkanImageViewType.h"
#include "vulkanImageType.h"
#include "vulkanMemoryPropertyFlag.h"
#include "vulkanRendererExport.h"
#include <memory>
#include <string>
#include <unordered_set>



// Forward declarations:
typedef struct VkImageView_T* VkImageView;



namespace vulkanRendererBackend
{
	// Forward declarations:
	class StagingBuffer;
	class VmaBuffer;
	class VmaImage;
	struct DeviceQueue;



	/// <summary>
	/// Base class for different kinds of textures: 1d, 2d, 3d, cube.
	/// </summary>
	class VULKAN_RENDERER_API Texture : public emberBackendInterface::ITexture
	{
	protected: // Static members:
		static std::unordered_set<Format> s_valid08BitFormats;
		static std::unordered_set<Format> s_valid16BitFormats;
		static std::unordered_set<Format> s_valid32BitFormats;
		static std::unordered_set<Format> s_valid64BitFormats;
		static std::unordered_set<Format> s_validSingleChannelFormats;
		static std::unordered_set<Format> s_validDoubleChannelFormats;
		static std::unordered_set<Format> s_validTripleChannelFormats;
		static std::unordered_set<Format> s_validQuadrupleChannelFormats;
		static std::unordered_set<Format> s_validDepthFormats;
		static std::unordered_set<Format> s_validStencilFormats;

	protected: // Members:
		std::string m_name;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_depth;
		uint32_t m_channels;
		Format m_format;
		DescriptorType m_descriptorType;
		std::unique_ptr<VmaImage> m_pImage;

	protected: // Methods:
		// Constructor:
		Texture();

	public: // Methods:
		// Destructor:
		virtual ~Texture();

		// Non-copyable:
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		// Movable:
		Texture(Texture&& other) noexcept;
		Texture& operator=(Texture&& other) noexcept;

		// Getters:
		const std::string& GetName() const override;
		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;
		uint32_t GetDepth() const override;
		uint32_t GetChannels() const override;
		const emberCommon::TextureFormat GetTextureFormat() const override;
		const VkImageView& GetVkImageView() const override;
		Format GetFormat() const;
		VmaImage* const GetVmaImage() const;
		DescriptorType GetDescriptorType() const;

	protected: // Methods:
		uint32_t GetChannelCount(Format format);
		uint32_t BytesPerChannel(Format format);
		bool IsValidImageFormat(Format format);
		bool IsDepthFormat(Format format);
		bool IsStencilFormat(Format format);
		void CreateImageBase(ImageType imageType, ImageSubresourceRange& subresourceRange, Format format, ImageUsageFlag usageFlags, ImageCreateFlag imageFlags, MemoryPropertyFlag memoryFlags, ImageViewType viewType, const DeviceQueue& queue);
	};
}