#pragma once
#include "vulkanTexture2d.h"



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
		StorageTexture2d(const std::string& name, VkFormat format, int width, int height, void* data);

		// Non-copyable:
		StorageTexture2d(const StorageTexture2d&) = delete;
		StorageTexture2d& operator=(const StorageTexture2d&) = delete;

		// Movable:
		StorageTexture2d(StorageTexture2d&& other) noexcept = default;
		StorageTexture2d& operator=(StorageTexture2d&& other) noexcept = default;

		~StorageTexture2d();
		void RecordGpuCommands(VkCommandBuffer& transferCommandBuffer, VkCommandBuffer& graphicsCommandBuffer, StagingBuffer* pStagingBuffer);

	private: // Methods:
		StagingBuffer* Load(const std::string& name, VkFormat format, const std::filesystem::path& path);
	};
}