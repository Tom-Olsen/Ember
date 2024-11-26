#pragma once
#ifndef __INCLUDE_GUARD_textureManager_h__
#define __INCLUDE_GUARD_textureManager_h__
#include <unordered_map>
#include <memory>
#include <string>
#include "texture2d.h"
#include "textureCube.h"



/// <summary>
/// Purely static class that takes care of lifetime of all Material objects.
/// </summary>
class TextureManager
{
public: // Members

private: // Members
    static bool isInitialized;
    static VulkanContext* context;
    static std::unordered_map<std::string, std::unique_ptr<Texture2d>> texture2ds;
    static std::unordered_map<std::string, std::unique_ptr<TextureCube>> textureCubes;

public: // Methods
    static void Init(VulkanContext* vulkanContext);
    static void Clear();

    static void AddTexture2d(Texture2d* texture);
    static void AddTextureCube(TextureCube* texture);
    static Texture2d* GetTexture2d(const std::string& name);
    static TextureCube* GetTextureCube(const std::string& name);
    static void DeleteTexture2d(const std::string& name);
    static void DeleteTextureCube(const std::string& name);

    static void PrintAllTextureNames();

private: // Methods
    // Delete all constructors:
    TextureManager() = delete;
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;
    ~TextureManager() = delete;
};



#endif // __INCLUDE_GUARD_textureManager_h__