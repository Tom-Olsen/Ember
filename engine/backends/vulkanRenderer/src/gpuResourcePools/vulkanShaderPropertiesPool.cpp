#include "vulkanShaderPropertiesPool.h"
#include "logger.h"
#include "vulkanShader.h"
#include "vulkanShaderProperties.h"



namespace vulkanRendererBackend
{
    // Constructor/Destructor:
    ShaderPropertiesPool::ShaderPropertiesPool()
    {
        m_currentUsage = 0;
        m_peakUsage = 0;
    }
    ShaderPropertiesPool::~ShaderPropertiesPool()
    {
        for (int i = 0; i < m_storage.size(); i++)
            delete m_storage[i];
    }


    // Public methods:
    ShaderProperties* ShaderPropertiesPool::CheckOut(Shader* pShader)
    {
        // Create new shader properties if pool is empty:
        if (m_pool.empty())
        {
            ShaderProperties* pNewShaderProperties = new ShaderProperties(pShader);
            m_storage.push_back(pNewShaderProperties);
            m_pool.push(pNewShaderProperties);
        }

        // Get resource and remove it from the queue:
        ShaderProperties* pShaderProperties = m_pool.front();
        m_pool.pop();

        // Update usage counts:
        m_currentUsage++;
        m_peakUsage = std::max(m_peakUsage, m_currentUsage);

        return pShaderProperties;
    }
    void ShaderPropertiesPool::Return(ShaderProperties* pShaderProperties)
    {
        m_pool.push(pShaderProperties);
        m_currentUsage--;
    }
    void ShaderPropertiesPool::ShrinkToFit()
    {
        if (m_currentUsage != 0)
        {
            LOG_WARN("Trying to shrink ShaderPropertiesPool which still has ShaderProperties checked out: currentUsage = {} != 0.", m_currentUsage);
            return;
        }

        // Resize storage:
        for (size_t i = m_peakUsage; i < m_storage.size(); i++)
            delete m_storage[i];
        m_storage.resize(m_peakUsage);

        // Rebuild queue:
        std::queue<ShaderProperties*> newPool;
        for (int i = 0; i < m_storage.size(); i++)
            newPool.push(m_storage[i]);
        std::swap(m_pool, newPool);
    }
}