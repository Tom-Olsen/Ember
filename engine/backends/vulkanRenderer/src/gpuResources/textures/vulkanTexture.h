#pragma once
#include "textureFormat.h"
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
	/// Base class for different kinds of textures: 1d, 2d, 3d, cube.
	/// </summary>
	class Texture
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
		std::string m_name;
		int m_width;
		int m_height;
		int m_depth;
		int m_channels;
		VkFormat m_format;
		VkDescriptorType m_descriptorType;
		std::unique_ptr<VmaImage> m_pImage;

	public: // Methods:
		// Constructor/Destructor:
		Texture() = default;
		virtual ~Texture();

		// Non-copyable:
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		// Movable:
		Texture(Texture&& other) noexcept = default;
		Texture& operator=(Texture&& other) noexcept = default;

		// Getters:
		const std::string& GetName() const;
		uint64_t GetWidth() const;
		uint64_t GetHeight() const;
		uint64_t GetDepth() const;
		uint64_t GetChannels() const;
		const emberEngine::TextureFormat& GetFormat() const;
		VkFormat GetVkFormat() const;
		VmaImage* const GetVmaImage() const;
		VkDescriptorType GetVkDescriptorType() const;

	protected: // Methods:
		uint32_t GetChannelCount(VkFormat format);
		uint32_t BytesPerChannel(VkFormat format);
		bool IsValidImageFormat(VkFormat format);
		bool IsDepthFormat(VkFormat format);
		bool IsStencilFormat(VkFormat format);
		void CreateImageBase(VkImageType imageType, VkImageSubresourceRange& subresourceRange, VkFormat format, VkImageUsageFlags usageFlags, VkImageCreateFlags imageFlags, VkMemoryPropertyFlags memoryFlags, VkImageViewType viewType, const DeviceQueue& queue);
	};
}