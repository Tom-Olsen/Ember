#ifndef __INCLUDE_GUARD_sampleTexture2d_h__
#define __INCLUDE_GUARD_sampleTexture2d_h__
#include "texture2d.h"



namespace emberEngine
{
	/// <summary>
	/// Texture2d specialization: <para/>
	/// -VkImageUsageFlags		= transfer src (for mipmap creation), transfer dst, storage, sample <para/>
	/// -VkImageCreateFlagBits	= 0 <para/>
	/// -VkImageViewType		= 2d
	/// </summary>
	class SampleTexture2d : public Texture2d
	{
	public: // Methods:
		SampleTexture2d(const std::filesystem::path& filePath, const std::string& name, VkFormat format);
		~SampleTexture2d();
	};
}



#endif // __INCLUDE_GUARD_sampleTexture2d_h__