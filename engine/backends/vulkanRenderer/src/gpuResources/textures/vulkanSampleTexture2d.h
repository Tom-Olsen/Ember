#pragma once
#include "vulkanTexture2d.h"
#include "emberMath.h"



namespace vulkanRendererBackend
{
	// Forward declerations:
	class StagingBuffer;
	class TextureBatchUploader;



	/// <summary>
	/// Texture2d specialization: <para/>
	/// -VkImageUsageFlags		= transfer src (for mipmap creation), transfer dst, storage, sample <para/>
	/// -VkImageCreateFlagBits	= 0 <para/>
	/// -VkImageViewType		= 2d
	/// </summary>
	class SampleTexture2d : public Texture2d
	{
	public: // Methods:
		SampleTexture2d(const std::string& name, int width, int height, float value);
		SampleTexture2d(const std::string& name, int width, int height, Float2 value);
		SampleTexture2d(const std::string& name, int width, int height, Float3 value);
		SampleTexture2d(const std::string& name, int width, int height, Float4 value);
		SampleTexture2d(const std::string& name, VkFormat format, const std::filesystem::path& path);
		SampleTexture2d(const std::string& name, VkFormat format, const std::filesystem::path& path, TextureBatchUploader& batchUploader);

		// Non-copyable:
		SampleTexture2d(const SampleTexture2d&) = delete;
		SampleTexture2d& operator=(const SampleTexture2d&) = delete;

		// DepthTexture2dArray:
		SampleTexture2d(SampleTexture2d&& other) noexcept = default;
		SampleTexture2d& operator=(SampleTexture2d&& other) noexcept = default;

		~SampleTexture2d();
		void RecordGpuCommands(VkCommandBuffer& transferCommandBuffer, VkCommandBuffer& graphicsCommandBuffer, StagingBuffer* pStagingBuffer) override;

	private: // Methods:
		void Init(StagingBuffer* pStagingBuffer);
		StagingBuffer* Load(const std::string& name, VkFormat format, const std::filesystem::path& path) override;
		StagingBuffer* Load(const std::string& name, int width, int height, float value);
		StagingBuffer* Load(const std::string& name, int width, int height, const Float2& value);
		StagingBuffer* Load(const std::string& name, int width, int height, const Float3& value);
		StagingBuffer* Load(const std::string& name, int width, int height, const Float4& value);
	};
}