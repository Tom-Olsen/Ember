#ifndef __INCLUDE_GUARD_poolManager_h__
#define __INCLUDE_GUARD_poolManager_h__
#include "shaderPropertiesPool.h"
#include <unordered_map>



namespace emberEngine
{
    // Forward declarations:
    class ComputeShader;



    /// <summary>
    /// Purely static class that takes care of lifetime of all managed buffer objects.
    /// </summary>
    class PoolManager
    {
    private: // Members
        static bool s_isInitialized;
		static std::unordered_map<ComputeShader*, ShaderPropertiesPool> s_shaderPropertiesPoolMap;

    public: // Methods
        static void Init();
        static void Clear();
        static ShaderProperties* CheckOutShaderProperties(ComputeShader* pComputeShader);
        static void ReturnShaderProperties(ComputeShader* pComputeShader, ShaderProperties* pShaderProperties);

    private: // Methods
        // Delete all constructors:
        PoolManager() = delete;
        PoolManager(const PoolManager&) = delete;
        PoolManager& operator=(const PoolManager&) = delete;
        ~PoolManager() = delete;
    };
}



#endif // __INCLUDE_GUARD_poolManager_h__