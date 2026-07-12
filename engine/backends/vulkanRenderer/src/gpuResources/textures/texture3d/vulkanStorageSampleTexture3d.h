#pragma once
#include "vulkanTexture3d.h"
#include "vulkanRendererExport.h"



namespace vulkanRendererBackend
{
	// Forward declarations:
	class StagingBuffer;



	/// <summary>
	/// Texture3d specialization: <para/>
	/// -VkImageUsageFlags	= transfer dst, storage, sample <para/>
	/// -VkImageCreateFlags	= 0 <para/>
	/// -VkImageViewType    = 3d
	/// </summary>
	class VULKAN_RENDERER_API StorageSampleTexture3d : public Texture3d
	{
	public: // Methods:
		// Constructor/Destructor:
		StorageSampleTexture3d(VkFormat format, int width, int height, int depth, void* data = nullptr);
		~StorageSampleTexture3d();

		// Non-copyable:
		StorageSampleTexture3d(const StorageSampleTexture3d&) = delete;
		StorageSampleTexture3d& operator=(const StorageSampleTexture3d&) = delete;

		// Movable:
		StorageSampleTexture3d(StorageSampleTexture3d&& other) noexcept;
		StorageSampleTexture3d& operator=(StorageSampleTexture3d&& other) noexcept;

		void SetData(void* data) override;

	private: // Methods:
		void Init(VkFormat format, int width, int height, int depth);
	};
}