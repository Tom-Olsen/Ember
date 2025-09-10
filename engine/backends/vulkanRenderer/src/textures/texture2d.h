#ifndef __INCLUDE_GUARD_vulkanRendererBackend_texture2d_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_texture2d_h__
#include <memory>
#include <string>
#include <filesystem>
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
	protected: // Members:
		int m_width;
		int m_height;
		int m_channels;
		std::string m_name;
		std::unique_ptr<VmaImage> m_pImage;
		VkDescriptorType m_descriptorType;

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
		uint64_t GetWidth() const;
		uint64_t GetHeight() const;
		uint64_t GetChannels() const;
		const std::string& GetName() const;
		VmaImage* const GetVmaImage() const;
		VkDescriptorType GetVkDescriptorType() const;

		virtual void RecordGpuCommands(VkCommandBuffer& transferCommandBuffer, VkCommandBuffer& graphicsCommandBuffer, StagingBuffer* pStagingBuffer) = 0;

	protected: // Methods:
		uint32_t BytesPerChannel(VkFormat format);
		void CreateImage(VkImageSubresourceRange& subresourceRange, VkFormat format, VkImageUsageFlags usageFlags, VkImageCreateFlags imageFlags, VkMemoryPropertyFlags memoryFlags, VkImageViewType viewType, const DeviceQueue& queue);
		virtual StagingBuffer* Load(const std::string& name, VkFormat format, const std::filesystem::path& path) = 0;
	};
}



#endif // __INCLUDE_GUARD_vulkanRendererBackend_texture2d_h__