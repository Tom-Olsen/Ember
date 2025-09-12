#include "stagingBufferPool.h"
#include "logger.h"
#include "stagingBuffer.h"



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
    void StagingBufferPool::ShrinkToFit()
    {
        if (m_currentUsage != 0)
        {
            LOG_WARN("Trying to shrink StagingBufferPool which still has StagingBuffers checked out: currentUsage  = {} != 0.", m_currentUsage);
            return;
        }

        // Resize storage:
        for (size_t i = m_peakUsage; i < m_storage.size(); i++)
            delete m_storage[i];
        m_storage.resize(m_peakUsage);

        // Rebuild queue:
        std::queue<StagingBuffer*> newPool;
        for (int i = 0; i < m_storage.size(); i++)
            newPool.push(m_storage[i]);
        std::swap(m_pool, newPool);
    }
}