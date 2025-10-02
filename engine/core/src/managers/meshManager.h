#pragma once
#include <unordered_map>
#include <memory>
#include <string>



namespace emberEngine
{
    // Forward declarations:
    class Mesh;



    /// <summary>
    /// Purely static class that takes care of lifetime of all Mesh objects.
    /// </summary>
    class MeshManager
    {
    public: // Members

    private: // Members
        static bool s_isInitialized;
        static std::unordered_map<std::string, std::unique_ptr<Mesh>> s_meshes;

    public: // Methods
        static void Init();
        //static void UnloadAllMeshes();    // ember::ToDo: not sure if this is needed
        static void Clear();

        static void AddMesh(Mesh&& mesh);  // must be called as AddMesh(std::move(mesh)). Leaves input mesh empty.
        static Mesh& GetMesh(const std::string& name);
        static Mesh* TryGetMesh(const std::string& name);
        static void DeleteMesh(const std::string& name);

        static void Print();

    private: // Methods
        // Delete all constructors:
        MeshManager() = delete;
        MeshManager(const MeshManager&) = delete;
        MeshManager& operator=(const MeshManager&) = delete;
        MeshManager(MeshManager&&) = delete;
        MeshManager& operator=(MeshManager&&) = delete;
        ~MeshManager() = delete;
    };
}