#pragma once
#include "commonMaterialType.h"
#include "emberCoreExport.h"
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>



// Forward decleration:
namespace emberBackendInterface
{
    class IMaterial;
}



namespace emberEngine
{
    // Forward declarations:
    class Material;



    /// <summary>
    /// Purely static class that takes care of lifetime of all Material objects.
    /// Material is a none-owning wrapper around IMaterial. The MaterialManager owns the IMaterial objects.
    /// </summary>
    class EMBER_CORE_API MaterialManager
    {
    private: // Members
        static bool s_isInitialized;
        static std::unordered_map<std::string, std::unique_ptr<emberBackendInterface::IMaterial>> s_materialInterfacesMap;

    public: // Methods
        // Initialization/Cleanup:
        static void Init();
        static void Clear();

        static Material CreateMaterial(emberCommon::MaterialType type, const std::string& name, uint32_t renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv = "");
        static Material GetMaterial(const std::string& name);       // throws on fail.
        static Material TryGetMaterial(const std::string& name);    // returns invalid Material on fail.
        static void DeleteMaterial(const std::string& name);

        // Debugging:
        static void Print();

    private: // Methods
        // Delete all constructors:
        MaterialManager() = delete;
        MaterialManager(const MaterialManager&) = delete;
        MaterialManager& operator=(const MaterialManager&) = delete;
        MaterialManager(MaterialManager&&) = delete;
        MaterialManager& operator=(MaterialManager&&) = delete;
        ~MaterialManager() = delete;
    };
}