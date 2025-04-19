#include "vulkanGarbageCollector.h"
#include "logger.h"
#include "vulkanContext.h"



namespace emberEngine
{
    namespace vulkanBackend
    {
        // Static members:
        bool GarbageCollector::s_isInitialized = false;
        std::deque<GarbageCollector::GarbageEntry> GarbageCollector::s_garbageQueue;



        // Initialization and cleanup:
        void GarbageCollector::Init()
        {
            if (s_isInitialized)
                return;
            s_isInitialized = true;
        }
        void GarbageCollector::Clear()
        {
            Context::WaitDeviceIdle();
            for (const GarbageEntry& entry : s_garbageQueue)
                entry.destroyCallback();
            s_garbageQueue.clear();
        }



        // Static methods:
        void GarbageCollector::Destroy(std::function<void()> destroyCallback)
        {
            s_garbageQueue.push_back(GarbageEntry{ Context::absoluteFrameIndex, std::move(destroyCallback) });
        }

        void GarbageCollector::CollectGarbage()
        {
            // garbageQueue is sortet. The first entry is always the oldest. Once we find the first entry that does not get deleted we can stop.
            while (!s_garbageQueue.empty())
            {
                const GarbageEntry& entry = s_garbageQueue.front();
                if (Context::absoluteFrameIndex >= entry.frameIndex + Context::framesInFlight)
                {
                    entry.destroyCallback();
                    s_garbageQueue.pop_front();
                }
                else
                    break;
            }
        }
    }
}