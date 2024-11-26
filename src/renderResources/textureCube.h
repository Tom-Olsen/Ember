#pragma once
#ifndef __INCLUDE_GUARD_textureCube_h__
#define __INCLUDE_GUARD_textureCube_h__
#include "texture2d.h"



class TextureCube : public Texture2d
{
public: // Methods:
	TextureCube(VulkanContext* context, const std::filesystem::path& folderPath, std::string name);
	~TextureCube();
};



#endif // __INCLUDE_GUARD_textureCube_h__