#pragma once
#include "vulkanTexture2d.h"
#include <filesystem>



namespace vulkanRendererBackend
{
	// Forward declerations:
	class StagingBuffer;



	/// <summary>
	/// Texture2d specialization: <para/>
	/// -ImageUsageFlag		= transfer dst, storage, sample <para/>
	/// -ImageCreateFlagBit	= 0 <para/>
	/// -ImageViewType		= 2d
	/// </summary>
	class StorageSampleTexture2d : public Texture2d
	{
	public: // Methods:
		// Constructor/Destructor:
		StorageSampleTexture2d(const std::string& name, Format format, int width, int height);
		StorageSampleTexture2d(const std::string& name, Format format, int width, int height, void* data);
		StorageSampleTexture2d(const std::string& name, Format format, const std::filesystem::path& path);
		~StorageSampleTexture2d();
		
		// Non-copyable:
		StorageSampleTexture2d(const StorageSampleTexture2d&) = delete;
		StorageSampleTexture2d& operator=(const StorageSampleTexture2d&) = delete;

		// Movable:
		StorageSampleTexture2d(StorageSampleTexture2d&& other) noexcept = default;
		StorageSampleTexture2d& operator=(StorageSampleTexture2d&& other) noexcept = default;

		void SetData(void* data) override;
        
    private: // Methods:
		void Init(const std::string& name, Format format, int width, int height);
		StagingBuffer* StageData(void* data);
		void Upload(StagingBuffer* pStagingBuffer);
	};
}