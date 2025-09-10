#ifndef __INCLUDE_GUARD_vulkanRendererBackend_storageTexture2d_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_storageTexture2d_h__
#include "texture2d.h"



namespace vulkanRendererBackend
{
	/// <summary>
	/// Texture2d specialization: <para/>
	/// -VkImageUsageFlags		= transfer dst, storage <para/>
	/// -VkImageCreateFlagBits	= 0 <para/>
	/// -VkImageViewType		= 2d
	/// </summary>
	class StorageTexture2d : public Texture2d
	{
	public: // Methods:
		StorageTexture2d(const std::string& name, VkFormat format, int width, int height);
		StorageTexture2d(const std::string& name, VkFormat format, const std::filesystem::path& filePath);

		// Non-copyable:
		StorageTexture2d(const StorageTexture2d&) = delete;
		StorageTexture2d& operator=(const StorageTexture2d&) = delete;

		// DepthTexture2dArray:
		StorageTexture2d(StorageTexture2d&& other) noexcept = default;
		StorageTexture2d& operator=(StorageTexture2d&& other) noexcept = default;

		~StorageTexture2d();
		void RecordGpuCommands(VkCommandBuffer& transferCommandBuffer, VkCommandBuffer& graphicsCommandBuffer, StagingBuffer* pStagingBuffer) override;

	private: // Methods:
		StagingBuffer* Load(const std::string& name, VkFormat format, const std::filesystem::path& path) override;
	};
}



#endif // __INCLUDE_GUARD_vulkanRendererBackend_storageTexture2d_h__