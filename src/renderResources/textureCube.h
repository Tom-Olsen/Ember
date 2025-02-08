#ifndef __INCLUDE_GUARD_textureCube_h__
#define __INCLUDE_GUARD_textureCube_h__
#include "texture2d.h"



namespace emberEngine
{
	class TextureCube : public Texture2d
	{
	public: // Methods:
		TextureCube(VulkanContext* pContext, const std::filesystem::path& folderPath, const std::string& name, VkFormat format);
		~TextureCube();
	};
}



#endif // __INCLUDE_GUARD_textureCube_h__