#include "vulkanGarbageCollector.h"
#include "vulkanContext.h"
#include <vulkan/vulkan.h>



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
            entry.collectGarbageCallback();
        s_garbageQueue.clear();
    }



    // Static methods:
    void GarbageCollector::RecordGarbage(std::function<void()> collectGarbageCallback)
    {
        s_garbageQueue.push_back(GarbageEntry{ Context::GetAbsoluteFrameIndex(), std::move(collectGarbageCallback)});
    }
    void GarbageCollector::CollectGarbage()
    {
        // Garbage queue is sortet. The first entry is always the oldest. Once we find the first entry that does not need cleanup we can stop.
        while (!s_garbageQueue.empty())
        {
            const GarbageEntry& entry = s_garbageQueue.front();
            if (Context::GetAbsoluteFrameIndex() >= entry.frameIndex + Context::GetFramesInFlight())
            {
                entry.collectGarbageCallback();
                s_garbageQueue.pop_front();
            }
            else
                break;
        }
    }
}