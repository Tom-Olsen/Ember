#ifndef __INCLUDE_GUARD_poolManager_h__
#define __INCLUDE_GUARD_poolManager_h__
#include "shaderPropertiesPool.h"
#include "stagingBufferPool.h"
#include <unordered_map>



namespace emberEngine
{
    // Forward declarations:
    class ComputeShader;
    class StagingBuffer;



    /// <summary>
    /// Purely static class that takes care of custom resource pools.
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
        // Delete all constructors:
        PoolManager() = delete;
        PoolManager(const PoolManager&) = delete;
        PoolManager& operator=(const PoolManager&) = delete;
        ~PoolManager() = delete;
    };
}



#endif // __INCLUDE_GUARD_poolManager_h__