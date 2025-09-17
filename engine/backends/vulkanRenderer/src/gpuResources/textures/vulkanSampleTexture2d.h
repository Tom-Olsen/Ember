#pragma once
#include "vulkanTexture2d.h"
#include <filesystem>



namespace vulkanRendererBackend
{
	// Forward declerations:
	class StagingBuffer;



	/// <summary>
	/// Texture2d specialization: <para/>
	/// -VkImageUsageFlags		= transfer src (for mipmap creation), transfer dst, storage, sample <para/>
	/// -VkImageCreateFlagBits	= 0 <para/>
	/// -VkImageViewType		= 2d
	/// </summary>
	class SampleTexture2d : public Texture2d
	{
	public: // Methods:
		// Constructor/Destructor
		SampleTexture2d(const std::string& name, VkFormat format, int width, int height, void* data);
		SampleTexture2d(const std::string& name, VkFormat format, const std::filesystem::path& path);
		~SampleTexture2d();

		// Non-copyable:
		SampleTexture2d(const SampleTexture2d&) = delete;
		SampleTexture2d& operator=(const SampleTexture2d&) = delete;

		// Movable:
		SampleTexture2d(SampleTexture2d&& other) noexcept = default;
		SampleTexture2d& operator=(SampleTexture2d&& other) noexcept = default;

		void RecordGpuCommands(VkCommandBuffer& transferCommandBuffer, VkCommandBuffer& graphicsCommandBuffer, StagingBuffer* pStagingBuffer);

	private: // Methods:
		void Init(StagingBuffer* pStagingBuffer);
		StagingBuffer* Upload(void* data);
	};
}