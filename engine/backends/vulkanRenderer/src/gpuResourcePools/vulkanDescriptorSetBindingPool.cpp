#include "vulkanDescriptorSetBindingPool.h"
#include "descriptorSetMacros.h"
#include "logger.h"
#include "vulkanShader.h"
#include "vulkanDescriptorSetBinding.h"



namespace vulkanRendererBackend
{
    // Public methods:
    // Constructor/Destructor:
    DescriptorSetBindingPool::DescriptorSetBindingPool()
    {
        m_currentUsage = 0;
        m_peakUsage = 0;
    }
    DescriptorSetBindingPool::~DescriptorSetBindingPool()
    {
        for (int i = 0; i < m_storage.size(); i++)
            delete m_storage[i];
    }


    // Checkout/Return:
    DescriptorSetBinding* DescriptorSetBindingPool::CheckOut(Shader* pShader)
    {
        // Create new shader properties if pool is empty:
        if (m_pool.empty())
        {
            DescriptorSetBinding* pNewDescriptorSetBinding = new DescriptorSetBinding(pShader, DRAW_SET_INDEX);
            m_storage.push_back(pNewDescriptorSetBinding);
            m_pool.push(pNewDescriptorSetBinding);
        }

        // Get resource and remove it from the queue:
        DescriptorSetBinding* pDescriptorSetBinding = m_pool.front();
        m_pool.pop();

        // Update usage counts:
        m_currentUsage++;
        m_peakUsage = std::max(m_peakUsage, m_currentUsage);

        return pDescriptorSetBinding;
    }
    void DescriptorSetBindingPool::Return(DescriptorSetBinding* pDescriptorSetBinding)
    {
        m_pool.push(pDescriptorSetBinding);
        m_currentUsage--;
    }



    // Debugging:
    void DescriptorSetBindingPool::PrintPoolState()
    {
        LOG_TRACE("   Stored DescriptorSetBinding count: {}", m_storage.size());
        LOG_TRACE("Available DescriptorSetBinding count: {}", m_pool.size());
    }
}