#ifndef __INCLUDE_GUARD_materialManager_h__
#define __INCLUDE_GUARD_materialManager_h__
#include <unordered_map>
#include <memory>
#include <string>



class Material;
struct VulkanContext;



/// <summary>
/// Purely static class that takes care of lifetime of all Material objects.
/// </summary>
class MaterialManager
{
private: // Members
    static bool s_isInitialized;
	static VulkanContext* s_pContext;
    static std::unordered_map<std::string, std::unique_ptr<Material>> s_materials;

public: // Methods
    static void Init(VulkanContext* pContext);
	static void Clear();

    static void AddMaterial(Material* pMaterial);
    static Material* GetMaterial(const std::string& name);
    static void DeleteMaterial(const std::string& name);

    static void PrintAllMaterialNames();

private: // Methods
    // Delete all constructors:
    MaterialManager() = delete;
    MaterialManager(const MaterialManager&) = delete;
    MaterialManager& operator=(const MaterialManager&) = delete;
    ~MaterialManager() = delete;
};



#endif // __INCLUDE_GUARD_materialManager_h__