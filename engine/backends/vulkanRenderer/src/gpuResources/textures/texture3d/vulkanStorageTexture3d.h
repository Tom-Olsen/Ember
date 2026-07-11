#pragma once
#include "vulkanTexture3d.h"
#include "vulkanRendererExport.h"



namespace vulkanRendererBackend
{
	// Forward declarations:
	class StagingBuffer;



	/// <summary>
	/// Texture3d specialization: <para/>
	/// -VkImageUsageFlags	= transfer dst, storage <para/>
	/// -VkImageCreateFlags	= 0 <para/>
	/// -VkImageViewType    = 3d
	/// </summary>
	class VULKAN_RENDERER_API StorageTexture3d : public Texture3d
	{
	public: // Methods:
		// Constructor/Destructor:
		StorageTexture3d(VkFormat format, int width, int height, int depth, void* data = nullptr);
		~StorageTexture3d();

		// Non-copyable:
		StorageTexture3d(const StorageTexture3d&) = delete;
		StorageTexture3d& operator=(const StorageTexture3d&) = delete;

		// Movable:
		StorageTexture3d(StorageTexture3d&& other) noexcept;
		StorageTexture3d& operator=(StorageTexture3d&& other) noexcept;

		void SetData(void* data) override;

	private: // Methods:
		void Init(VkFormat format, int width, int height, int depth);
		StagingBuffer* StageData(void* data);
		void Upload(StagingBuffer* pStagingBuffer);
	};
}
