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
}