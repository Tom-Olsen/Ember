#ifndef __INCLUDE_GUARD_storageSampleTexture2d_h__
#define __INCLUDE_GUARD_storageSampleTexture2d_h__
#include "texture2d.h"



namespace emberEngine
{
	/// <summary>
	/// Texture2d specialization: <para/>
	/// -VkImageUsageFlags		= transfer dst, storage <para/>
	/// -VkImageCreateFlagBits	= 0 <para/>
	/// -VkImageViewType		= 2d
	/// </summary>
	class StorageSampleTexture2d : public Texture2d
	{
	public: // Methods:
		StorageSampleTexture2d(const std::string& name, VkFormat format, int width, int height);
		StorageSampleTexture2d(const std::filesystem::path& filePath, const std::string& name, VkFormat format);
		~StorageSampleTexture2d();
	};
}



#endif // __INCLUDE_GUARD_storageSampleTexture2d_h__