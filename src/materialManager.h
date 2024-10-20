#pragma once
#ifndef __INCLUDE_GUARD_materialManager_h__
#define __INCLUDE_GUARD_materialManager_h__
#include <unordered_map>
#include <memory>
#include <string>
#include "material.h"



/// <summary>
/// Purely static class that takes care of lifetime of all Material objects.
/// </summary>
class MaterialManager
{
public: // Members

private: // Members
	static VulkanContext* context;
    static std::unordered_map<std::string, std::unique_ptr<Material>> materials;

public: // Methods
    static void InitDefaultMaterials(VulkanContext* vulkanContext);
	static void ClearMaterialMap();

    static void AddMaterial(const std::string name, Material* material);
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