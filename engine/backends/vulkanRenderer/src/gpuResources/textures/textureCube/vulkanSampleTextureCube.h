#pragma once
#include "vulkanTextureCube.h"
#include <filesystem>



// Forward declerations:
typedef struct VkCommandBuffer_T* VkCommandBuffer;



namespace vulkanRendererBackend
{
	// Forward decleration:
	class StagingBuffer;



	/// <summary>
	/// SampleTextureCube specialization: <para/>
	/// -ImageUsageFlag		= transfer src/dst, sample <para/>
	/// -ImageCreateFlagBit	= cube compatible <para/>
	/// -ImageViewType		= 2d
	/// </summary>
	class SampleTextureCube : public TextureCube
	{
	public: // Methods:
		// Constructor/Destructor:
		SampleTextureCube(const std::string& name, Format format, int width, int height);
		SampleTextureCube(const std::string& name, Format format, int width, int height, void* data);
		SampleTextureCube(const std::string& name, Format format, const std::filesystem::path& path);
		~SampleTextureCube();

		// Non-copyable:
		SampleTextureCube(const SampleTextureCube&) = delete;
		SampleTextureCube& operator=(const SampleTextureCube&) = delete;

		// Movable:
		SampleTextureCube(SampleTextureCube&& other) noexcept = default;
		SampleTextureCube& operator=(SampleTextureCube&& other) noexcept = default;

		void SetData(void* data) override;

	private: // Methods:
		void Init(const std::string& name, Format format, int width, int height);
		StagingBuffer* StageData(void* data);
		void Upload(StagingBuffer* pStagingBuffer);
		void RecordGpuCommands(VkCommandBuffer transferCommandBuffer, VkCommandBuffer graphicsCommandBuffer, StagingBuffer* pStagingBuffer);
	};
}