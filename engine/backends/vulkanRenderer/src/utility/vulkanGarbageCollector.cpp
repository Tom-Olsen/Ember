#include "vulkanGarbageCollector.h"
#include "logger.h"
#include "vulkanContext.h"
#include <utility>
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
        Flush();
        s_isInitialized = false;
    }
    void GarbageCollector::Flush()
    {
        // Wait for device idle before flushing queued callbacks:
        if (!Context::IsDeviceIdle())
        {
            LOG_WARN("GarbageCollector::Flush() called while device may still be busy. Waiting for device idle before clearing queued Vulkan resources.");
            Context::WaitDeviceIdle();
        }

        // Callbacks may enqueue more garbage while destroying owning resources.
        // Pop before invoking each callback so queue mutations cannot affect the active entry.
        while (!s_garbageQueue.empty())
        {
            std::function<void()> collectGarbageCallback = std::move(s_garbageQueue.front().collectGarbageCallback);
            s_garbageQueue.pop_front();
            collectGarbageCallback();
        }
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
        	// Callbacks may enqueue more garbage while destroying owning resources.
        	// Pop before invoking each callback so queue mutations cannot affect the active entry
            if (Context::GetAbsoluteFrameIndex() >= s_garbageQueue.front().frameIndex + Context::GetFramesInFlight())
            {
                std::function<void()> collectGarbageCallback = std::move(s_garbageQueue.front().collectGarbageCallback);
                s_garbageQueue.pop_front();
                collectGarbageCallback();
            }
            else
                break;
        }
    }
}