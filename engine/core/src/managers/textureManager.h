#pragma once
#include "commonTextureId.h"
#include "commonTextureType.h"
#include "emberCoreExport.h"
#include "namedSlotMap.h"
#include "texture.h"
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>



// Forward declarations:
namespace emberAssetLoader
{
    struct TextureAsset;
}
namespace emberBackendInterface
{
    class ITexture;
}



namespace emberCore
{
	// Forward declarations:
    class Core;
    class Renderer;
    class Texture2d;
    class Texture3d;
    class TextureCube;
    class Texture2DOwner;
    class Texture3DOwner;
    class TextureCubeOwner;



    class EMBER_CORE_API TextureManager
    {
		// Friends:
        friend class Core;
        friend class Renderer;
        friend class Texture;
        friend class Texture2d;
        friend class Texture3d;
        friend class TextureCube;
        friend class Texture2DOwner;
        friend class Texture3DOwner;
        friend class TextureCubeOwner;

    private: // Structs:
        struct ManagedTexture
        {
            bool ownedByTextureManager;
            emberCommon::TextureType textureType;
            std::unique_ptr<emberBackendInterface::ITexture> pITexture;

			// Constructor/Destructor:
            ManagedTexture(bool ownedByTextureManager, emberCommon::TextureType textureType, emberBackendInterface::ITexture* pITexture);
            ~ManagedTexture();
			
			// Non-copyable:
            ManagedTexture(const ManagedTexture&) = delete;
            ManagedTexture& operator=(const ManagedTexture&) = delete;
			
			// Movable:
            ManagedTexture(ManagedTexture&&) noexcept;
            ManagedTexture& operator=(ManagedTexture&&) noexcept;
        };

    private: // Members:
		static bool s_isInitialized;
        static const std::string s_finalRenderTextureName;
        static const std::string s_gizmoTextureName;
        static emberDataStructures::NamedSlotMap<emberCommon::TextureId, ManagedTexture> s_textureSlotMap;

    public: // Methods:
        // Asset loading:
        static void LoadTextureAssets(const std::filesystem::path& directoryPath);

        // Getters:
        static Texture GetTexture(const std::string& name);
        template<typename T>
        static T GetTexture(const std::string& name);
        static Texture TryGetTexture(const std::string& name);

        // Deleter:
        static void DeleteTexture(const std::string& name);

        // Debugging:
        static void Print();

    private: // Methods:
        // Initialization/Cleanup:
        static void Init();
        static void Clear();

        // Asset creation:
        static emberCommon::TextureId CreateTexture(const emberAssetLoader::TextureAsset& textureAsset);

        // Management:
        // Takes ownership of pITexture, including on failure.
        static emberCommon::TextureId AddTexture(const std::string& name, emberBackendInterface::ITexture* pITexture, emberCommon::TextureType textureType);
        static emberBackendInterface::ITexture* TryGetTextureInterface(emberCommon::TextureId textureId);
        static std::string GetTextureName(emberCommon::TextureId textureId);
        static emberCommon::TextureType GetTextureType(emberCommon::TextureId textureId);
        static void DeleteTexture(emberCommon::TextureId textureId);
        static void RetireTexture(emberBackendInterface::ITexture* pITexture);
        static std::string CreateUniqueTextureName(const std::string& name);	// Used for owning texture wrapper so same name can be reused.

        // Delete all constructors:
        TextureManager() = delete;
        TextureManager(const TextureManager&) = delete;
        TextureManager& operator=(const TextureManager&) = delete;
        TextureManager(TextureManager&&) = delete;
        TextureManager& operator=(TextureManager&&) = delete;
        ~TextureManager() = delete;
    };
}