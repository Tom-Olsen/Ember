#ifndef __INCLUDE_GUARD_sampleTexture2d_h__
#define __INCLUDE_GUARD_sampleTexture2d_h__
#include "texture2d.h"



namespace emberEngine
{
	// Forward declerations:
	namespace vulkanBackend
	{
		class TextureBatchUploader;
	}
	/// <summary>
	/// Texture2d specialization: <para/>
	/// -VkImageUsageFlags		= transfer src (for mipmap creation), transfer dst, storage, sample <para/>
	/// -VkImageCreateFlagBits	= 0 <para/>
	/// -VkImageViewType		= 2d
	/// </summary>
	class SampleTexture2d : public Texture2d
	{
	public: // Methods:
		SampleTexture2d(const std::string& name, VkFormat format, const std::filesystem::path& filePath);
		SampleTexture2d(const std::string& name, VkFormat format, const std::filesystem::path& filePath, vulkanBackend::TextureBatchUploader batchUploader);
		~SampleTexture2d();
	};
}



#endif // __INCLUDE_GUARD_sampleTexture2d_h__