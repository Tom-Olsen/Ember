#ifndef __INCLUDE_GUARD_textureCube_h__
#define __INCLUDE_GUARD_textureCube_h__
#include "texture2d.h"



namespace emberEngine
{
	/// <summary>
	/// Texture2d specialization: <para/>
	/// -VkImageUsageFlags		= transfer src/dst, sample <para/>
	/// -VkImageCreateFlagBits	= cube compatible <para/>
	/// -VkImageViewType		= cube
	/// </summary>
	class TextureCube : public Texture2d
	{
	public: // Methods:
		TextureCube(VulkanContext* pContext, const std::filesystem::path& folderPath, const std::string& name, VkFormat format);
		~TextureCube();
	};
}



#endif // __INCLUDE_GUARD_textureCube_h__