#ifndef __INCLUDE_GUARD_bufferManager_h__
#define __INCLUDE_GUARD_bufferManager_h__
#include <unordered_map>
#include <memory>
#include <string>



namespace emberEngine
{
    // Forward declarations:
    class Buffer;

    

    /// <summary>
    /// Purely static class that takes care of lifetime of all managed buffer objects.
    /// </summary>
    class BufferManager
    {
    private: // Members
        static bool s_isInitialized;
        static std::unordered_map<std::string, std::unique_ptr<Buffer>> s_buffers;

    public: // Methods
        static void Init();
        static void Clear();

        static void AddBuffer(Buffer* pBuffer, const std::string& name);
        static Buffer* GetBuffer(const std::string& name);
        static void DeleteBuffer(const std::string& name);

    private: // Methods
        // Delete all constructors:
        BufferManager() = delete;
        BufferManager(const BufferManager&) = delete;
        BufferManager& operator=(const BufferManager&) = delete;
        BufferManager(BufferManager&&) = delete;
        BufferManager& operator=(BufferManager&&) = delete;
        ~BufferManager() = delete;
    };
}



#endif // __INCLUDE_GUARD_bufferManager_h__