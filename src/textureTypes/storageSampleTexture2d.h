#ifndef __INCLUDE_GUARD_storageSampleTexture2d_h__
#define __INCLUDE_GUARD_storageSampleTexture2d_h__
#include "texture2d.h"



namespace emberEngine
{
	/// <summary>
	/// Texture2d specialization: <para/>
	/// -VkImageUsageFlags		= transfer dst, storage
	/// -VkImageCreateFlagBits	= 0
	/// -VkImageViewType		= 2d
	/// </summary>
	class StorageSampleTexture2d : public Texture2d
	{
	public: // Methods:
		StorageSampleTexture2d(VulkanContext* pContext, const std::string& name, VkFormat format, int width, int height);
		StorageSampleTexture2d(VulkanContext* pContext, const std::filesystem::path& filePath, const std::string& name, VkFormat format);
		~StorageSampleTexture2d();
	};
}



#endif // __INCLUDE_GUARD_storageSampleTexture2d_h__