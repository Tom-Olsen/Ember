#ifndef __INCLUDE_GUARD_vulkanRendererBackend_shaderPropertiesPool_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_shaderPropertiesPool_h__
#include <memory>
#include <queue>
#include <vector>



namespace vulkanRendererBackend
{
    // Forward declerations:
    class Shader;
    class ShaderProperties;



    /// <summary>
    /// Use one shader properties pool per Shader*.
    /// </summary>
    class ShaderPropertiesPool
    {
    private: // Members:
        std::vector<ShaderProperties*> m_storage;   // Stores ShaderProperties*
        std::queue<ShaderProperties*> m_pool;       // Queue of available ShaderProperties*.
        size_t m_currentUsage;  // Tracks how many shader properties have been handed out at any moment.
        size_t m_peakUsage;     // Tracks peak usage.

    public: // Methods:
        ShaderPropertiesPool();
        ~ShaderPropertiesPool();

        // Non-copyable:
        ShaderPropertiesPool(const ShaderPropertiesPool&) = delete;
        ShaderPropertiesPool& operator=(const ShaderPropertiesPool&) = delete;

        // Movable:
        ShaderPropertiesPool(ShaderPropertiesPool&& other) noexcept = default;
        ShaderPropertiesPool& operator=(ShaderPropertiesPool&& other) noexcept = default;

        ShaderProperties* CheckOut(Shader* pShader);
        void Return(ShaderProperties* pShaderProperties);
        void ShrinkToFit(); // Should only ever be called if current usage is 0.
    };
}



#endif // __INCLUDE_GUARD_vulkanRendererBackend_shaderPropertiesPool_h__