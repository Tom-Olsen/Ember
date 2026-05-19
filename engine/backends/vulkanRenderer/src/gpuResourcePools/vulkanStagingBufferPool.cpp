#include "vulkanStagingBufferPool.h"
#include "logger.h"
#include "vulkanStagingBuffer.h"
#include <algorithm>



namespace vulkanRendererBackend
{
    // Public methods:
    // Constructor/Destructor:
    StagingBufferPool::StagingBufferPool(uint16_t availableStorageBuffers)
    {
        m_availableStorageBuffers = availableStorageBuffers;
        m_currentUsage = 0;
        m_peakUsage = 0;
    }
    StagingBufferPool::~StagingBufferPool()
    {
        for (int i = 0; i < m_storage.size(); i++)
            delete m_storage[i];
    }


    // Checkout/Return:
    StagingBuffer* StagingBufferPool::CheckOut(uint32_t size)
    {
        // Create new staging buffer if pool is empty:
        if (m_pool.empty())
        {
            StagingBuffer* pNewStagingBuffer = new StagingBuffer(size);
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
        if (pStagingBuffer == nullptr)
        {
            LOG_ERROR("StagingBufferPool::Return(...) failed. pStagingBuffer is nullptr.");
            return;
        }
        if (!Owns(pStagingBuffer))
        {
            LOG_ERROR("StagingBufferPool::Return(...) failed. pStagingBuffer does not belong to this pool.");
            return;
        }
        if (m_currentUsage == 0)
        {
            LOG_ERROR("StagingBufferPool::Return(...) failed. No staging buffers are currently checked out.");
            return;
        }
        if (IsAvailable(pStagingBuffer))
        {
            LOG_ERROR("StagingBufferPool::Return(...) failed. pStagingBuffer has already been returned.");
            return;
        }

        m_currentUsage--;
        if (m_pool.size() >= m_availableStorageBuffers)
        {
            Destroy(pStagingBuffer);
            return;
        }
        m_pool.push(pStagingBuffer);
    }



    // Debugging:
    void StagingBufferPool::PrintPoolState()
    {
        LOG_TRACE("   Stored StagingBuffers count: {}", m_storage.size());
        LOG_TRACE("Available StagingBuffers count: {}", m_pool.size());
    }



    // Private methods:
    bool StagingBufferPool::Owns(StagingBuffer* pStagingBuffer) const
    {
        return std::find(m_storage.begin(), m_storage.end(), pStagingBuffer) != m_storage.end();
    }
    bool StagingBufferPool::IsAvailable(StagingBuffer* pStagingBuffer) const
    {
        std::queue<StagingBuffer*> pool = m_pool;
        while (!pool.empty())
        {
            if (pool.front() == pStagingBuffer)
                return true;
            pool.pop();
        }
        return false;
    }
    void StagingBufferPool::Destroy(StagingBuffer* pStagingBuffer)
    {
        auto it = std::find(m_storage.begin(), m_storage.end(), pStagingBuffer);
        if (it == m_storage.end())
            return;

        delete pStagingBuffer;
        m_storage.erase(it);
    }
}