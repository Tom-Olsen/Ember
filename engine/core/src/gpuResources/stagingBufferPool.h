#ifndef __INCLUDE_GUARD_stagingBufferPool_h__
#define __INCLUDE_GUARD_stagingBufferPool_h__
#include "logger.h"
#include <memory>
#include <queue>    // FIFO queue
#include <vector>



// Ember::TODO:
// -remove ShrinkToFit? some other mechanism to handle unbound growth?
// -same for ShaderPropertiesPool.
namespace emberEngine
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

        StagingBuffer* CheckOut(uint32_t size);
        void Return(StagingBuffer* pStagingBuffer);
        void ShrinkToFit(); // Should only ever be called if current usage is 0.
    };
}



#endif // __INCLUDE_GUARD_stagingBufferPool_h__