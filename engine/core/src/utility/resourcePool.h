#pragma once
#include "logger.h"
#include <queue>    // FIFO queue
#include <vector>



// Ember::TODO:
// -add blockSize to shaderPropertiesPool?
// -delete this file/class entirely?
// -currently not used at all! debrecated? If used in vulkanBackend only, move it there.



namespace emberEngine
{
    /// <summary>
    /// Header only class template for easy use without explicit template instantiation.
    /// Resource Pool dynamically growths in blockSize steps.
    /// When your resource needs inputs for its constructor, they can be handed over with the variadic template Args&&... args in Acquire().
    /// </summary>
    template <typename T, size_t blockSize>
    class ResourcePool
    {
    private: // Members:
        std::vector<std::vector<T>> m_storageBlocks; // Stores all resources in N/blockSize many contiguous memory blocks.
        std::queue<T*> m_pool;  // Queue of available resource pointers.
        size_t m_currentUsage;  // Tracks how many resources have been handed out at any moment.
        size_t m_peakUsage;     // Tracks the peak of resources that have ever been handed out.

    public: // Methods:
        template <typename... Args>
        T* Acquire(Args&&... args)
        {
            if (m_pool.empty())
                AddBlock(std::forward<Args>(args)...);

            // Get resource and remove it from the queue:
            T* resource = m_pool.front();
            m_pool.pop();

            // Update usage counts:
            m_currentUsage++;
            m_peakUsage = std::max(m_peakUsage, m_currentUsage);

            return resource;
        }

        void Release(T* resource)
        {
            m_pool.push(resource);
            m_currentUsage--;
        }

        // Should only ever be called if current usage is 0!
        void ShrinkToFit()
        {
            if (m_currentUsage != 0)
            {
                LOG_WARN("Trying to shrink resourcePool which is still in use (currentUsage != 0).");
                return;
            }

            // Compute number of needed blocks and reset peak usage:
            size_t neededBlocks = (m_peakUsage + blockSize - 1) / blockSize;
            m_peakUsage = 0;

            // Remove excess blocks iteratively:
            while (m_storageBlocks.size() > neededBlocks)
                m_storageBlocks.pop_back();

            // Rebuild queue:
            std::queue<T*> newPool;
            for (auto& block : m_storageBlocks)
                for (T& obj : block)
                    newPool.push(&obj);

            // Replace old queue with new queue:
            std::swap(m_pool, newPool);
        }

    private: // Methods:
        template <typename... Args>
        void AddBlock(Args&&... args)
        {
            // Add a new block:
            m_storageBlocks.emplace_back();
            std::vector<T>& lastBlock = m_storageBlocks.back();
            lastBlock.reserve(blockSize);

            // Add new block to pool:
            for (int i = 0; i < blockSize; i++)
            {
                lastBlock.emplace_back(std::forward<Args>(args)...);
                m_pool.push(&lastBlock[i]);
            }
        }
    };
}