#ifndef __INCLUDE_GUARD_poolManager_h__
#define __INCLUDE_GUARD_poolManager_h__
#include "shaderPropertiesPool.h"
#include "stagingBufferPool.h"
#include <map>
#include <unordered_map>



namespace vulkanRendererBackend
{
    // Forward declarations:
    class Shader;
    class ComputeShader;
    class StagingBuffer;



    /// <summary>
    /// Purely static class that takes care of custom resource pools.
    /// ShaderProperties and StagingBuffers are special, as they get one pool per:
    /// -ShaderProperties: Shader (material or computeShader)
    /// -StagingBuffer: size (rounded up to next power of two)
    /// </summary>
    class PoolManager
    {
    private: // Members
        static bool s_isInitialized;
		static std::unordered_map<Shader*, ShaderPropertiesPool> s_shaderPropertiesPoolMap;
        static std::map<uint32_t, StagingBufferPool> s_stagingBufferPoolMap;

    public: // Methods
        static void Init();
        static void Clear();
        static ShaderProperties* CheckOutShaderProperties(Shader* pShader);
        static StagingBuffer* CheckOutStagingBuffer(uint32_t size);
        static void ReturnShaderProperties(Shader* pShader, ShaderProperties* pShaderProperties);
        static void ReturnStagingBuffer(uint32_t size, StagingBuffer* pStagingBuffer);

    private: // Methods
        static uint32_t NextPowerOfTwo(uint32_t n);

        // Delete all constructors:
        PoolManager() = delete;
        PoolManager(const PoolManager&) = delete;
        PoolManager& operator=(const PoolManager&) = delete;
        PoolManager(PoolManager&&) = delete;
        PoolManager& operator=(PoolManager&&) = delete;
        ~PoolManager() = delete;
    };
}



#endif // __INCLUDE_GUARD_poolManager_h__