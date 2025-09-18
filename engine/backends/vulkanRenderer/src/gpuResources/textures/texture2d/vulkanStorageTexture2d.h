#pragma once
#include "vulkanTexture2d.h"
#include <filesystem>



namespace vulkanRendererBackend
{
	// Forward declerations:
	class StagingBuffer;



	/// <summary>
	/// Texture2d specialization: <para/>
	/// -VkImageUsageFlags		= transfer dst, storage <para/>
	/// -VkImageCreateFlagBits	= 0 <para/>
	/// -VkImageViewType		= 2d
	/// </summary>
	class StorageTexture2d : public Texture2d
	{
	public: // Methods:
		// Constructor/Destructor:
		StorageTexture2d(const std::string& name, VkFormat format, int width, int height, void* data);
		StorageTexture2d(const std::string& name, VkFormat format, const std::filesystem::path& path);
		~StorageTexture2d();

		// Non-copyable:
		StorageTexture2d(const StorageTexture2d&) = delete;
		StorageTexture2d& operator=(const StorageTexture2d&) = delete;

		// Movable:
		StorageTexture2d(StorageTexture2d&& other) noexcept = default;
		StorageTexture2d& operator=(StorageTexture2d&& other) noexcept = default;

	private: // Methods:
		StagingBuffer* Staging(void* data);
		void Upload(StagingBuffer* pStagingBuffer);
	};
}