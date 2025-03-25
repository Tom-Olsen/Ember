#include "vulkanGarbageCollector.h"
#include "vulkanContext.h"



namespace emberEngine
{
	// Static members:
	bool VulkanGarbageCollector::s_isInitialized = false;
	std::deque<VulkanGarbageCollector::GarbageEntry> VulkanGarbageCollector::s_garbageQueue;



	// Initialization and cleanup:
	void VulkanGarbageCollector::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;
	}
	void VulkanGarbageCollector::Clear()
	{
		VulkanContext::WaitDeviceIdle();
        for (const GarbageEntry& entry : s_garbageQueue)
            entry.destroyCallback();
        s_garbageQueue.clear();
	}
	


	// Static methods:
    void VulkanGarbageCollector::Destroy(std::function<void()> destroyCallback)
    {
        s_garbageQueue.push_back(GarbageEntry{VulkanContext::absoluteFrameIndex, std::move(destroyCallback)});
    }

    void VulkanGarbageCollector::CollectGarbage()
    {
        // garbageQueue is sortet. The first entry is always the oldest. Once we find the first entry that does not get deleted we can stop.
        while (!s_garbageQueue.empty())
        {
            const GarbageEntry& entry = s_garbageQueue.front();
            if (VulkanContext::absoluteFrameIndex >= entry.frameIndex + VulkanContext::framesInFlight)
            {
                entry.destroyCallback();
                s_garbageQueue.pop_front();
            }
            else
                break;
        }
    }
}