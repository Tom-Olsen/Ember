#ifndef __INCLUDE_GUARD_storageTexture2d_h__
#define __INCLUDE_GUARD_storageTexture2d_h__
#include "texture2d.h"



namespace emberEngine
{
	/// <summary>
	/// Texture2d specialization: <para/>
	/// -VkImageUsageFlags		= transfer src/dst, storage
	/// -VkImageCreateFlagBits	= 0
	/// -VkImageViewType		= 2d
	/// </summary>
	class StorageTexture2d : public Texture2d
	{
	public: // Methods:
		StorageTexture2d(VulkanContext* pContext, const std::string& name, VkFormat format, int width, int height);
		StorageTexture2d(VulkanContext* pContext, const std::filesystem::path& filePath, const std::string& name, VkFormat format);
		~StorageTexture2d();
	};
}



#endif // __INCLUDE_GUARD_storageTexture2d_h__