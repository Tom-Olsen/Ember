#include "vulkanStagingBufferPool.h"
#include "vulkanStagingBuffer.h"



namespace vulkanRendererBackend
{
    // Constructor/Destructor:
    StagingBufferPool::StagingBufferPool()
    {
        m_currentUsage = 0;
        m_peakUsage = 0;
    }
    StagingBufferPool::~StagingBufferPool()
    {
        for (int i = 0; i < m_storage.size(); i++)
            delete m_storage[i];
    }


    // Public methods:
    StagingBuffer* StagingBufferPool::CheckOut(uint32_t size)
    {
        // Create new staging buffer if pool is empty:
        if (m_pool.empty())
        {
            StagingBuffer* pNewStagingBuffer = new StagingBuffer(size, "pool");
            m_storage.push_back(pNewStagingBuffer);
            m_pool.push(pNewStagingBuffer);
        }

        // Get resource and remove it from the queue:
        StagingBuffer* pStagingBuffer = m_pool.front();
        m_pool.pop();

        // Update usage counts:
        m_currentUsage++;
        m_peakUsage = std::max(m_peakUsage, m_currentUsage);

        return pStagingBuffer;
    }
    void StagingBufferPool::Return(StagingBuffer* pStagingBuffer)
    {
        m_pool.push(pStagingBuffer);
        m_currentUsage--;
    }
}