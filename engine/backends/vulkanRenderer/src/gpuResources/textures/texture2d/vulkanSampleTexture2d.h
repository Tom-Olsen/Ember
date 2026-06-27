#pragma once
#include "vulkanTexture2d.h"
#include "vulkanRendererExport.h"
#include <filesystem>
#include <string>



// Forward declarations:
typedef struct VkCommandBuffer_T* VkCommandBuffer;



namespace vulkanRendererBackend
{
	// Forward declarations:
	class StagingBuffer;



	/// <summary>
	/// Texture2d specialization: <para/>
	/// -VkImageUsageFlags	= transfer src (for mipmap creation), transfer dst, storage, sample <para/>
	/// -VkImageCreateFlags	= 0 <para/>
	/// -VkImageViewType    = 2d
	/// </summary>
	class VULKAN_RENDERER_API SampleTexture2d : public Texture2d
	{
	public: // Methods:
		// Constructor/Destructor:
		SampleTexture2d(VkFormat format, int width, int height, void* data = nullptr);
		~SampleTexture2d();

		// Non-copyable:
		SampleTexture2d(const SampleTexture2d&) = delete;
		SampleTexture2d& operator=(const SampleTexture2d&) = delete;

		// Movable:
		SampleTexture2d(SampleTexture2d&& other) noexcept;
		SampleTexture2d& operator=(SampleTexture2d&& other) noexcept;

		void SetData(void* data) override;

    private: // Methods:
		void Init(VkFormat format, int width, int height);
		StagingBuffer* StageData(void* data);
		void Upload(StagingBuffer* pStagingBuffer);
		void RecordGpuCommands(VkCommandBuffer transferCommandBuffer, VkCommandBuffer graphicsCommandBuffer, StagingBuffer* pStagingBuffer);
	};
}