#pragma once
#include <deque>
#include <functional>



namespace vulkanRendererBackend
{
    class GarbageCollector
    {
    private:
        struct GarbageEntry
        {
            uint64_t frameIndex;
            std::function<void()> cleanupCallback;
        };

        static bool s_isInitialized;
        static std::deque<GarbageEntry> s_garbageQueue;

    public:
        static void Init();
        static void Clear();
        static void RecordCleanup(std::function<void()> cleanupCallback);
        static void Cleanup();
    };
}