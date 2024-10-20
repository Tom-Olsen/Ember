#pragma once
#ifndef __INCLUDE_GUARD_meshManager_h__
#define __INCLUDE_GUARD_meshManager_h__
#include <unordered_map>
#include <memory>
#include <string>
#include "mesh.h"



/// <summary>
/// Purely static class that takes care of lifetime of all static Mesh objects.
/// </summary>
class MeshManager
{
public: // Members

private: // Members
	static bool isInitialized;
    static VulkanContext* context;
    static std::unordered_map<std::string, std::unique_ptr<Mesh>> meshes;

public: // Methods
    static void Init(VulkanContext* vulkanContext);
    static void UnloadAllMeshes();
    static void Clear();

    static void AddMesh(const std::string name, Mesh* mesh);
    static Mesh* GetMesh(const std::string& name);
    static void DeleteMesh(const std::string& name);

    static void PrintAllMeshNames();

private: // Methods
	// Delete all constructors:
    MeshManager() = delete;
    MeshManager(const MeshManager&) = delete;
    MeshManager& operator=(const MeshManager&) = delete;
    ~MeshManager() = delete;

	// Default mesh constructors:
	static Mesh* UnitQuad();
    static Mesh* UnitCube();
};



#endif // __INCLUDE_GUARD_meshManager_h__