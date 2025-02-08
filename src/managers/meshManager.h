#ifndef __INCLUDE_GUARD_meshManager_h__
#define __INCLUDE_GUARD_meshManager_h__
#include <unordered_map>
#include <memory>
#include <string>



namespace emberEngine
{
    // Forward declarations:
    class Mesh;
    struct VulkanContext;



    /// <summary>
    /// Purely static class that takes care of lifetime of all Mesh objects.
    /// </summary>
    class MeshManager
    {
    public: // Members

    private: // Members
        static bool s_isInitialized;
        static VulkanContext* s_pContext;
        static std::unordered_map<std::string, std::unique_ptr<Mesh>> s_meshes;

    public: // Methods
        static void Init(VulkanContext* pContext);
        static void UnloadAllMeshes();
        static void Clear();

        static void AddMesh(Mesh* pMesh);
        static Mesh* GetMesh(const std::string& name);
        static void DeleteMesh(const std::string& name);

        static void PrintAllMeshNames();

    private: // Methods
        // Delete all constructors:
        MeshManager() = delete;
        MeshManager(const MeshManager&) = delete;
        MeshManager& operator=(const MeshManager&) = delete;
        ~MeshManager() = delete;
    };
}



#endif // __INCLUDE_GUARD_meshManager_h__