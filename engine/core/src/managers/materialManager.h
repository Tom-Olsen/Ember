#pragma once
#include <unordered_map>
#include <memory>
#include <string>



namespace emberEngine
{
    // Forward declarations:
    class Material;



    /// <summary>
    /// Purely static class that takes care of lifetime of all Material objects.
    /// </summary>
    class MaterialManager
    {
    private: // Members
        static bool s_isInitialized;
        static std::unordered_map<std::string, std::unique_ptr<Material>> s_materials;

    public: // Methods
        static void Init();
        static void Clear();

        static void AddMaterial(Material&& material); // must be called as AddMaterial(std::move(material)). Leaves input material empty.
        static Material& GetMaterial(const std::string& name);
        static Material* TryGetMaterial(const std::string& name);
        static void DeleteMaterial(const std::string& name);

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