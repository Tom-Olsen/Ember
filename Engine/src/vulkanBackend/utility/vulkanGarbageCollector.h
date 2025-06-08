#ifndef __INCLUDE_GUARD_vulkanGarbageCollector_h__
#define __INCLUDE_GUARD_vulkanGarbageCollector_h__
#include <vulkan/vulkan.h>
#include <deque>
#include <functional>



namespace emberEngine
{
    namespace vulkanBackend
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
}



#endif // __INCLUDE_GUARD_vulkanGarbageCollector_h__