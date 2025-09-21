#pragma once
#include <memory>
#include <queue>
#include <vector>



namespace vulkanRendererBackend
{
    // Forward declerations:
    class StagingBuffer;


    /// <summary>
    /// Use one staging buffer pool per size.
    /// </summary>
    class StagingBufferPool
    {
    private: // Members:
        std::vector<StagingBuffer*> m_storage;   // Stores StagingBuffer*
        std::queue<StagingBuffer*> m_pool;       // Queue of available StagingBuffer*.
        size_t m_currentUsage;  // Tracks how many staging buffers have been handed out at any moment.
        size_t m_peakUsage;     // Tracks peak usage.

    public: // Methods:
        StagingBufferPool();
        ~StagingBufferPool();

        // Non-copyable:
        StagingBufferPool(const StagingBufferPool&) = delete;
        StagingBufferPool& operator=(const StagingBufferPool&) = delete;

        // Movable:
        StagingBufferPool(StagingBufferPool&& other) noexcept = default;
        StagingBufferPool& operator=(StagingBufferPool&& other) noexcept = default;

        StagingBuffer* CheckOut(uint32_t size);
        void Return(StagingBuffer* pStagingBuffer);
    };
}