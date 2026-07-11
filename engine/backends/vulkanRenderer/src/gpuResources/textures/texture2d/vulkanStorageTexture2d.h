#pragma once
#include "vulkanTexture2d.h"
#include "vulkanRendererExport.h"



namespace vulkanRendererBackend
{
	// Forward declarations:
	class StagingBuffer;



	/// <summary>
	/// Texture2d specialization: <para/>
	/// -VkImageUsageFlags	= transfer dst, storage <para/>
	/// -VkImageCreateFlags	= 0 <para/>
	/// -VkImageViewType    = 2d
	/// </summary>
	class VULKAN_RENDERER_API StorageTexture2d : public Texture2d
	{
	public: // Methods:
		// Constructor/Destructor:
		StorageTexture2d(VkFormat format, int width, int height, void* data = nullptr);
		~StorageTexture2d();

		// Non-copyable:
		StorageTexture2d(const StorageTexture2d&) = delete;
		StorageTexture2d& operator=(const StorageTexture2d&) = delete;

		// Movable:
		StorageTexture2d(StorageTexture2d&& other) noexcept;
		StorageTexture2d& operator=(StorageTexture2d&& other) noexcept;

		void SetData(void* data) override;

	private: // Methods:
		void Init(VkFormat format, int width, int height);
		StagingBuffer* StageData(void* data);
		void Upload(StagingBuffer* pStagingBuffer);
	};
}