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
	/// Polymorphic parent class for different kinds of 2d textures.
	/// Each derivative handles a different type of VkImage.
	/// The images differ in: <para/>
	/// -VkImageUsageFlags		=> transfer, sampling, storage, depth, color. <para/>
	/// -VkImageCreateFlagBits	=> additional specialisations. <para/>
	/// -VkImageViewType		=> single image, array, cube.
	/// </summary>
	class Texture2d
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
		int m_channels;
		VkFormat m_format;
		VkDescriptorType m_descriptorType;
		std::unique_ptr<VmaImage> m_pImage;

	public: // Methods:
		Texture2d() = default;
		virtual ~Texture2d();

		// Non-copyable:
		Texture2d(const Texture2d&) = delete;
		Texture2d& operator=(const Texture2d&) = delete;

		// Movable:
		Texture2d(Texture2d&& other) noexcept = default;
		Texture2d& operator=(Texture2d&& other) noexcept = default;

		// Getters:
		const std::string& GetName() const;
		uint64_t GetWidth() const;
		uint64_t GetHeight() const;
		uint64_t GetChannels() const;
		const emberEngine::TextureFormat& GetFormat() const;
		VkFormat GetVkFormat() const;
		VmaImage* const GetVmaImage() const;
		VkDescriptorType GetVkDescriptorType() const;

	protected: // Methods:
        uint32_t GetChannelCount(VkFormat format);
		uint32_t BytesPerChannel(VkFormat format);
		void CreateImage(VkImageSubresourceRange& subresourceRange, VkFormat format, VkImageUsageFlags usageFlags, VkImageCreateFlags imageFlags, VkMemoryPropertyFlags memoryFlags, VkImageViewType viewType, const DeviceQueue& queue);

    public: // Static methods:
	};
}