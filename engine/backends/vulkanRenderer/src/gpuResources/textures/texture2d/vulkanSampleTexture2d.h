#pragma once
#include "vulkanTexture2d.h"
#include <filesystem>



// Forward declerations:
typedef struct VkCommandBuffer_T* VkCommandBuffer;



namespace vulkanRendererBackend
{
	// Forward declerations:
	class StagingBuffer;



	/// <summary>
	/// Texture2d specialization: <para/>
	/// -ImageUsageFlag		= transfer src (for mipmap creation), transfer dst, storage, sample <para/>
	/// -ImageCreateFlagBit	= 0 <para/>
	/// -ImageViewType		= 2d
	/// </summary>
	class SampleTexture2d : public Texture2d
	{
	public: // Methods:
		// Constructor/Destructor:
		SampleTexture2d(const std::string& name, Format format, int width, int height, void* data);
		SampleTexture2d(const std::string& name, Format format, const std::filesystem::path& path);
		~SampleTexture2d();

		// Non-copyable:
		SampleTexture2d(const SampleTexture2d&) = delete;
		SampleTexture2d& operator=(const SampleTexture2d&) = delete;

		// Movable:
		SampleTexture2d(SampleTexture2d&& other) noexcept = default;
		SampleTexture2d& operator=(SampleTexture2d&& other) noexcept = default;

    private: // Methods:
		StagingBuffer* Staging(void* data);
		void Upload(StagingBuffer* pStagingBuffer);
		void RecordGpuCommands(VkCommandBuffer transferCommandBuffer, VkCommandBuffer graphicsCommandBuffer, StagingBuffer* pStagingBuffer);
	};
}