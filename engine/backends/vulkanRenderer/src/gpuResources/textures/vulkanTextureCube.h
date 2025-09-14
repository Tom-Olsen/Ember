#pragma once
#include "emberMath.h"
#include "vulkanTexture2d.h"



namespace vulkanRendererBackend
{
	// Forward declerations:
	class StagingBuffer;
	class TextureBatchUploader;



	/// <summary>
	/// Texture2d specialization: <para/>
	/// -VkImageUsageFlags		= transfer src/dst, sample <para/>
	/// -VkImageCreateFlagBits	= cube compatible <para/>
	/// -VkImageViewType		= cube
	/// -path = folder path containing 6 images, nx ny nz px pyp z
	/// </summary>
	class TextureCube : public Texture2d
	{
	public: // Methods:
		TextureCube(const std::string& name, Float4 color);
		TextureCube(const std::string& name, VkFormat format, const std::filesystem::path& path);
		TextureCube(const std::string& name, VkFormat format, const std::filesystem::path& path, TextureBatchUploader& batchUploader);

		// Non-copyable:
		TextureCube(const TextureCube&) = delete;
		TextureCube& operator=(const TextureCube&) = delete;

		// DepthTexture2dArray:
		TextureCube(TextureCube&& other) noexcept = default;
		TextureCube& operator=(TextureCube&& other) noexcept = default;
		
		~TextureCube();
		void RecordGpuCommands(VkCommandBuffer& transferCommandBuffer, VkCommandBuffer& graphicsCommandBuffer, StagingBuffer* pStagingBuffer) override;

	private: // Methods:
		void Init(StagingBuffer* pStagingBuffer);
		StagingBuffer* Load(const std::string& name, VkFormat format, const std::filesystem::path& path) override;
		StagingBuffer* Load(const std::string& name, const Float4& color);
	};
}