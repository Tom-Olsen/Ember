#pragma once
#include "emberCoreExport.h"
#include <unordered_map>
#include <memory>
#include <string>



namespace emberEngine
{
    // Forward declarations:
    class Texture;



    /// <summary>
    /// Purely static class that takes care of lifetime of all Texture objects.
    /// </summary>
    class EMBER_CORE_API TextureManager
    {
    private: // Members:
        static bool s_isInitialized;
        static std::unordered_map<std::string, std::unique_ptr<Texture>> s_textures;

    public: // Methods:
        static void Init();
        static void Clear();

        static void AddTexture(Texture&& texture); // must be called as AddTexture(std::move(texture)). Leaves input texture empty.
        static Texture& GetTexture(const std::string& name);
        static Texture* TryGetTexture(const std::string& name);
        static void DeleteTexture(const std::string& name);

        static void Print();

    private: // Methods:
        // Delete all constructors:
        TextureManager() = delete;
        TextureManager(const TextureManager&) = delete;
        TextureManager& operator=(const TextureManager&) = delete;
        TextureManager(TextureManager&&) = delete;
        TextureManager& operator=(TextureManager&&) = delete;
        ~TextureManager() = delete;
    };
}