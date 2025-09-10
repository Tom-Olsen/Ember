#ifndef __INCLUDE_GUARD_vulkanRendererBackend_sampleTexture2d_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_sampleTexture2d_h__
#include "texture2d.h"
#include "iMath.h"



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
	/// -path = path to image
	/// </summary>
	class SampleTexture2d : public Texture2d
	{
	public: // Methods:
		SampleTexture2d(const std::string& name, float value);
		SampleTexture2d(const std::string& name, iMath::Float4 color);
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
		StagingBuffer* Load(const std::string& name, float value);
		StagingBuffer* Load(const std::string& name, const iMath::Float4& color);
	};
}



#endif // __INCLUDE_GUARD_vulkanRendererBackend_sampleTexture2d_h__