#include "vulkanGarbageCollector.h"
#include "logger.h"
#include "vulkanContext.h"
#include <stdexcept>
#include <utility>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
    // Static members:
    bool GarbageCollector::s_isInitialized = false;
    std::deque<GarbageCollector::FrameGarbageEntry> GarbageCollector::s_frameGarbageQueue;
    std::vector<GarbageCollector::PendingGarbageEntry> GarbageCollector::s_pendingGarbage;



	// Public methods:
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

		// Device is idle, so all pending pending garbage should be save to clear:
		CollectPendingGarbage();
		if (!s_pendingGarbage.empty())
    		throw std::runtime_error("GarbageCollector::Flush() failed. Pending garbage is not ready for deletion.");
        CollectFrameGarbage(true);
    }



	// Record garbage:
    void GarbageCollector::RecordFrameGarbage(std::function<void()> collectGarbageCallback)
    {
        s_frameGarbageQueue.push_back(FrameGarbageEntry{ Context::GetAbsoluteFrameIndex(), std::move(collectGarbageCallback) });
    }
	void GarbageCollector::RecordPendingGarbage(std::function<bool()> collectGarbageCallback)
	{
        s_pendingGarbage.push_back(PendingGarbageEntry{ std::move(collectGarbageCallback) });
	}



	// Collect garbage:
    void GarbageCollector::CollectGarbage()
	{
		CollectFrameGarbage();
		CollectPendingGarbage();
	}
    void GarbageCollector::CollectFrameGarbage(bool force)
    {
        // Garbage queue is sortet. The first entry is always the oldest. Once we find the first entry that does not need cleanup we can stop.
        while (!s_frameGarbageQueue.empty())
        {
        	// Callbacks may enqueue more garbage while destroying owning resources.
        	// Pop before invoking each callback so queue mutations cannot affect the active entry
            if (force || Context::GetAbsoluteFrameIndex() >= s_frameGarbageQueue.front().frameIndex + Context::GetFramesInFlight() + 1)
            {
                std::function<void()> collectGarbageCallback = std::move(s_frameGarbageQueue.front().collectGarbageCallback);
                s_frameGarbageQueue.pop_front();
                collectGarbageCallback();
            }
            else
                break;
        }
    }
	void GarbageCollector::CollectPendingGarbage()
	{
		// Move pending garbage into local vector and clear it:
	    std::vector<PendingGarbageEntry> pendingGarbage = std::move(s_pendingGarbage);
	    s_pendingGarbage.clear();

		// Clear pending garbage that is rdy and rerecord those that are not:
	    for (PendingGarbageEntry& garbage : pendingGarbage)
	    {
	        if (!garbage.collectGarbageCallback())
	            s_pendingGarbage.push_back(std::move(garbage));
	    }
	}
}