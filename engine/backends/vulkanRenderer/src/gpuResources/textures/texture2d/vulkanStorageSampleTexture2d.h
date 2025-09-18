#pragma once
#include "vulkanTexture2d.h"
#include <filesystem>



namespace vulkanRendererBackend
{
	// Forward declerations:
	class StagingBuffer;



	/// <summary>
	/// Texture2d specialization: <para/>
	/// -VkImageUsageFlags		= transfer dst, storage, sample <para/>
	/// -VkImageCreateFlagBits	= 0 <para/>
	/// -VkImageViewType		= 2d
	/// </summary>
	class StorageSampleTexture2d : public Texture2d
	{
	public: // Methods:
		// Constructor/Destructor:
		StorageSampleTexture2d(const std::string& name, VkFormat format, int width, int height, void* data);
		StorageSampleTexture2d(const std::string& name, VkFormat format, const std::filesystem::path& path);
		~StorageSampleTexture2d();
		
		// Non-copyable:
		StorageSampleTexture2d(const StorageSampleTexture2d&) = delete;
		StorageSampleTexture2d& operator=(const StorageSampleTexture2d&) = delete;

		// Movable:
		StorageSampleTexture2d(StorageSampleTexture2d&& other) noexcept = default;
		StorageSampleTexture2d& operator=(StorageSampleTexture2d&& other) noexcept = default;

        
    private: // Methods:
		StagingBuffer* Staging(void* data);
		void Upload(StagingBuffer* pStagingBuffer);
	};
}