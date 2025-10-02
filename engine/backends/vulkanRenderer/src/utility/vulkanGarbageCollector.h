#pragma once
#include <cstdint>
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
            std::function<void()> collectGarbageCallback;
        };

        static bool s_isInitialized;
        static std::deque<GarbageEntry> s_garbageQueue;

    public:
        static void Init();
        static void Clear();
        static void RecordGarbage(std::function<void()> collectGarbageCallback);
        static void CollectGarbage();
    };
}