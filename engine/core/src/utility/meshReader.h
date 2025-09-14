#ifndef __INCLUDE_GUARD_meshReader_h__
#define __INCLUDE_GUARD_meshReader_h__
#include <filesystem>



namespace emberEngine
{
    // Forward declarations:
    class Mesh;



    struct MeshReader
    {
    private:
        static bool s_isInitialized;

    public: // Methods:
        static Mesh* ReadCsvFile(const std::filesystem::path& relativePath);

    private: // Methods
        // Delete all constructors:
        MeshReader() = delete;
        MeshReader(const MeshReader&) = delete;
        MeshReader& operator=(const MeshReader&) = delete;
        ~MeshReader() = delete;
    };
}



#endif // __INCLUDE_GUARD_meshReader_h__