#include "vulkanShaderPropertiesPool.h"
#include "logger.h"
#include "vulkanShader.h"
#include "vulkanShaderProperties.h"



namespace vulkanRendererBackend
{
    // Public methods:
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


    // Checkout/Return:
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



    // Debugging:
    void ShaderPropertiesPool::PrintPoolState()
    {
        LOG_TRACE("   Stored ShaderProperties count: {}", m_storage.size());
        LOG_TRACE("Available ShaderProperties count: {}", m_pool.size());
    }
}