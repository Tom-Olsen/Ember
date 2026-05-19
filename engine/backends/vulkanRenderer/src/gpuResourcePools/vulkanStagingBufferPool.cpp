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
        m_availableStagingBuffers.reserve(availableStorageBuffers);
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
        StagingBuffer* pStagingBuffer;

        // Create new staging buffer if pool is empty:
        if (m_pool.empty())
        {
            pStagingBuffer = new StagingBuffer(size);
            m_storage.push_back(pStagingBuffer);
        }
        // Get resource and remove it from the queue:
        else
        {
            pStagingBuffer = m_pool.front();
            m_pool.pop();
            m_availableStagingBuffers.erase(pStagingBuffer);
        }

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
        m_availableStagingBuffers.insert(pStagingBuffer);
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
        return m_availableStagingBuffers.find(pStagingBuffer) != m_availableStagingBuffers.end();
    }
    void StagingBufferPool::Destroy(StagingBuffer* pStagingBuffer)
    {
        auto it = std::find(m_storage.begin(), m_storage.end(), pStagingBuffer);
        if (it == m_storage.end())
            return;

        m_availableStagingBuffers.erase(pStagingBuffer);
        delete pStagingBuffer;
        m_storage.erase(it);
    }
}