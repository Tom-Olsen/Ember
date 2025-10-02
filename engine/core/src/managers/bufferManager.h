#pragma once
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

        static void AddBuffer(Buffer&& buffer); // must be called as AddBuffer(std::move(buffer)). Leaves input buffer empty.
        static Buffer& GetBuffer(const std::string& name);
        static Buffer* TryGetBuffer(const std::string& name);
        static void DeleteBuffer(const std::string& name);

        static void Print();

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