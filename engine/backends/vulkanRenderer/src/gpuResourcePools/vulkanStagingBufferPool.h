#pragma once
#include <cstdint>
#include <memory>
#include <queue>
#include <vector>



namespace vulkanRendererBackend
{
    // Forward declarations:
    class StagingBuffer;

    

    /// <summary>
    /// Use one staging buffer pool per size.
    /// </summary>
    class StagingBufferPool
    {
    private: // Members:
        uint16_t m_availableStorageBuffers;  // Number of storage buffers in the pool. More can be requested, but will be destroyed on return.
        std::vector<StagingBuffer*> m_storage;  // Stores StagingBuffer*
        std::queue<StagingBuffer*> m_pool;      // Queue of available StagingBuffer*.
        size_t m_currentUsage;  // Tracks how many staging buffers have been handed out at any moment.
        size_t m_peakUsage;     // Tracks peak usage.

    public: // Methods:
        StagingBufferPool(uint16_t availableStorageBuffers);
        ~StagingBufferPool();

        // Non-copyable:
        StagingBufferPool(const StagingBufferPool&) = delete;
        StagingBufferPool& operator=(const StagingBufferPool&) = delete;

        // Movable:
        StagingBufferPool(StagingBufferPool&& other) noexcept = default;
        StagingBufferPool& operator=(StagingBufferPool&& other) noexcept = default;

        // Checkout/Return:
        StagingBuffer* CheckOut(uint32_t size);
        void Return(StagingBuffer* pStagingBuffer);

        // Debugging:
        void PrintPoolState();

    private: // Methods:
        bool Owns(StagingBuffer* pStagingBuffer) const;
        bool IsAvailable(StagingBuffer* pStagingBuffer) const;
        void Destroy(StagingBuffer* pStagingBuffer);
    };
}