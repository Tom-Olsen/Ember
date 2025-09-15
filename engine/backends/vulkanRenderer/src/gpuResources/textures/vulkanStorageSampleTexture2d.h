#pragma once
#include "vulkanTexture2d.h"



namespace vulkanRendererBackend
{
	/// <summary>
	/// Texture2d specialization: <para/>
	/// -VkImageUsageFlags		= transfer dst, storage, sample <para/>
	/// -VkImageCreateFlagBits	= 0 <para/>
	/// -VkImageViewType		= 2d
	/// </summary>
	class StorageSampleTexture2d : public Texture2d
	{
	public: // Methods:
		StorageSampleTexture2d(const std::string& name, VkFormat format, int width, int height);
		StorageSampleTexture2d(const std::string& name, VkFormat format, const std::filesystem::path& filePath);
		
		// Non-copyable:
		StorageSampleTexture2d(const StorageSampleTexture2d&) = delete;
		StorageSampleTexture2d& operator=(const StorageSampleTexture2d&) = delete;

		// DepthTexture2dArray:
		StorageSampleTexture2d(StorageSampleTexture2d&& other) noexcept = default;
		StorageSampleTexture2d& operator=(StorageSampleTexture2d&& other) noexcept = default;

		~StorageSampleTexture2d();
		void RecordGpuCommands(VkCommandBuffer& transferCommandBuffer, VkCommandBuffer& graphicsCommandBuffer, StagingBuffer* pStagingBuffer) override;

	private: // Methods:
		StagingBuffer* Load(const std::string& name, VkFormat format, const std::filesystem::path& path) override;
	};
}