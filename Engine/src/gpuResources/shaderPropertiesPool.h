#ifndef __INCLUDE_GUARD_shaderPropertiesPool_h__
#define __INCLUDE_GUARD_shaderPropertiesPool_h__
#include "logger.h"
#include <memory>
#include <queue>    // FIFO queue
#include <vector>



namespace emberEngine
{
    // Forward declerations:
    class Shader;
    class ShaderProperties;



    class ShaderPropertiesPool
    {
    private: // Members:
        std::vector<ShaderProperties*> m_storage;   // Stores ShaderProperties*
        std::queue<ShaderProperties*> m_pool;       // Queue of available ShaderProperties*.
        size_t m_currentUsage;  // Tracks how many shaderProperties have been handed out at any moment.
        size_t m_peakUsage;     // Tracks peak usage.

    public: // Methods:
        ShaderPropertiesPool();
        ~ShaderPropertiesPool();

        ShaderProperties* CheckOut(Shader* pShader);
        void Return(ShaderProperties* pShaderProperties);
        void ShrinkToFit(); // Should only ever be called if current usage is 0!
    };
}



#endif // __INCLUDE_GUARD_shaderPropertiesPool_h__