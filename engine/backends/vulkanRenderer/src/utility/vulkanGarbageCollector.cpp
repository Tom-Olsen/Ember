#include "vulkanGarbageCollector.h"
#include "vulkanContext.h"



namespace vulkanRendererBackend
{
    // Static members:
    bool GarbageCollector::s_isInitialized = false;
    std::deque<GarbageCollector::GarbageEntry> GarbageCollector::s_garbageQueue;



    // Initialization/Cleanup:
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
            entry.cleanupCallback();
        s_garbageQueue.clear();
    }



    // Static methods:
    void GarbageCollector::RecordCleanup(std::function<void()> cleanupCallback)
    {
        s_garbageQueue.push_back(GarbageEntry{ Context::absoluteFrameIndex, std::move(cleanupCallback) });
    }
    void GarbageCollector::Cleanup()
    {
        // Garbage queue is sortet. The first entry is always the oldest. Once we find the first entry that does not need cleanup we can stop.
        while (!s_garbageQueue.empty())
        {
            const GarbageEntry& entry = s_garbageQueue.front();
            if (Context::absoluteFrameIndex >= entry.frameIndex + Context::framesInFlight)
            {
                entry.cleanupCallback();
                s_garbageQueue.pop_front();
            }
            else
                break;
        }
    }
}