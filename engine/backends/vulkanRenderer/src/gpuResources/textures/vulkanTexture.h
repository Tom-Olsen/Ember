#pragma once
#include "iTexture.h"
#include "commonTextureFormat.h"
#include "vulkanRendererExport.h"
#include <memory>
#include <string>
#include <unordered_set>
#include <vulkan/vulkan.h>



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
		static std::unordered_set<VkFormat> s_valid08BitFormats;
		static std::unordered_set<VkFormat> s_valid16BitFormats;
		static std::unordered_set<VkFormat> s_valid32BitFormats;
		static std::unordered_set<VkFormat> s_valid64BitFormats;
		static std::unordered_set<VkFormat> s_validSingleChannelFormats;
		static std::unordered_set<VkFormat> s_validDoubleChannelFormats;
		static std::unordered_set<VkFormat> s_validTripleChannelFormats;
		static std::unordered_set<VkFormat> s_validQuadrupleChannelFormats;
		static std::unordered_set<VkFormat> s_validDepthFormats;
		static std::unordered_set<VkFormat> s_validStencilFormats;

	protected: // Members:
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_depth;
		uint32_t m_channels;
		VkFormat m_format;
		VkDescriptorType m_vkDescriptorType;
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
		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;
		uint32_t GetDepth() const override;
		uint32_t GetChannels() const override;
		const emberCommon::TextureFormat GetTextureFormat() const override;
		const VkImageView& GetVkImageView() const override;
		VkFormat GetFormat() const;
		VmaImage* const GetVmaImage() const;
		VkDescriptorType GetVkDescriptorType() const;

        // Debugging:
        void SetDebugName(const std::string& name) override;

	protected: // Methods:
		uint32_t GetChannelCount(VkFormat format);
		uint32_t BytesPerChannel(VkFormat format);
		bool IsValidImageFormat(VkFormat format);
		bool IsDepthFormat(VkFormat format);
		bool IsStencilFormat(VkFormat format);
		uint64_t GetImageSize(const VkImageSubresourceRange& subresourceRange, VkFormat format);
		void CreateImageBase(VkImageType imageType, VkImageSubresourceRange& subresourceRange, VkFormat format, VkImageUsageFlags usageFlags, VkImageCreateFlags imageFlags, VkMemoryPropertyFlags memoryFlags, VkImageViewType viewType, const DeviceQueue& queue);
	};
}