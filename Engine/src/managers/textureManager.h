#ifndef __INCLUDE_GUARD_textureManager_h__
#define __INCLUDE_GUARD_textureManager_h__
#include <unordered_map>
#include <memory>
#include <string>



namespace emberEngine
{
    // Forward declarations:
    class Texture2d;



    /// <summary>
    /// Purely static class that takes care of lifetime of all Texture objects.
    /// </summary>
    class TextureManager
    {
    private: // Members:
        static bool s_isInitialized;
        static std::unordered_map<std::string, std::unique_ptr<Texture2d>> s_texture2ds;

    public: // Methods:
        static void Init();
        static void Clear();

        static void AddTexture2d(Texture2d* pTexture);
        static Texture2d* GetTexture2d(const std::string& name);
        static void DeleteTexture2d(const std::string& name);

        static void PrintAllTextureNames();

    private: // Methods:
        // Delete all constructors:
        TextureManager() = delete;
        TextureManager(const TextureManager&) = delete;
        TextureManager& operator=(const TextureManager&) = delete;
        ~TextureManager() = delete;
    };
}



#endif // __INCLUDE_GUARD_textureManager_h__