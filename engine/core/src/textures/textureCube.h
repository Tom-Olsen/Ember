#ifndef __INCLUDE_GUARD_textureCube_h__
#define __INCLUDE_GUARD_textureCube_h__
#include "texture2d.h"



namespace emberEngine
{
	// Forward declerations:
	class StagingBuffer;
	namespace vulkanBackend
	{
		class TextureBatchUploader;
	}



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
		TextureCube(const std::string& name, VkFormat format, const std::filesystem::path& path);
		TextureCube(const std::string& name, VkFormat format, const std::filesystem::path& path, vulkanBackend::TextureBatchUploader& batchUploader);
		~TextureCube();

	private: // Methods:
		StagingBuffer* Init(const std::string& name, VkFormat format, const std::filesystem::path& path) override;
	public: // Methods:
		void RecordGpuCommands(VkCommandBuffer& transferCommandBuffer, VkCommandBuffer& graphicsCommandBuffer, StagingBuffer* pStagingBuffer) override;
	};
}



#endif // __INCLUDE_GUARD_textureCube_h__