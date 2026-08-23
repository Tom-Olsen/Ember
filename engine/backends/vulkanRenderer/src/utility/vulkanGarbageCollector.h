#pragma once
#include <cstdint>
#include <deque>
#include <functional>
#include <vector>



namespace vulkanRendererBackend
{
    class GarbageCollector
    {
    private: // Structs:
        struct FrameGarbageEntry
        {
            uint64_t frameIndex;
            std::function<void()> collectGarbageCallback;
        };
		struct PendingGarbageEntry
		{
			std::function<bool()> collectGarbageCallback;	// must return true when deletion was succesfull.
		};

    private: // Members:
        static bool s_isInitialized;
        static std::deque<FrameGarbageEntry> s_frameGarbageQueue;
		static std::vector<PendingGarbageEntry> s_pendingGarbage;

    public: // Methods:
        // Initialization/Cleanup:
        static void Init();
        static void Clear();
        static void Flush();

		// Record garbage:
        static void RecordFrameGarbage(std::function<void()> collectGarbageCallback);
        static void RecordPendingGarbage(std::function<bool()> collectGarbageCallback);

		// Collect garbage:
        static void CollectGarbage();
        static void CollectFrameGarbage(bool force = false);
        static void CollectPendingGarbage();

    private: // Methods:
        // Delete all constructors:
        GarbageCollector() = delete;
        GarbageCollector(const GarbageCollector&) = delete;
        GarbageCollector& operator=(const GarbageCollector&) = delete;
        GarbageCollector(GarbageCollector&&) = delete;
        GarbageCollector& operator=(GarbageCollector&&) = delete;
        ~GarbageCollector() = delete;
    };
}