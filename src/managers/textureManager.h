#ifndef __INCLUDE_GUARD_textureManager_h__
#define __INCLUDE_GUARD_textureManager_h__
#include <unordered_map>
#include <memory>
#include <string>



namespace emberEngine
{
    // Forward declarations:
    class Texture2d;
    class TextureCube;
    struct VulkanContext;



    /// <summary>
    /// Purely static class that takes care of lifetime of all Texture objects.
    /// </summary>
    class TextureManager
    {
    private: // Members
        static bool s_isInitialized;
        static VulkanContext* s_pContext;
        static std::unordered_map<std::string, std::unique_ptr<Texture2d>> s_texture2ds;
        static std::unordered_map<std::string, std::unique_ptr<TextureCube>> s_textureCubes;

    public: // Methods
        static void Init(VulkanContext* pContext);
        static void Clear();

        static void AddTexture2d(Texture2d* pTexture);
        static void AddTextureCube(TextureCube* pTexture);
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
}



#endif // __INCLUDE_GUARD_textureManager_h__