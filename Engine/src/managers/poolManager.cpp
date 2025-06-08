#include "poolManager.h"
#include "emberMath.h"
#include "logger.h"
#include "macros.h"



namespace emberEngine
{
	// Static members:
	bool PoolManager::s_isInitialized = false;
	std::unordered_map<Shader*, ShaderPropertiesPool> PoolManager::s_shaderPropertiesPoolMap;
	std::map<uint32_t, StagingBufferPool> PoolManager::s_stagingBufferPoolMap;



	// Initialization/Cleanup:
	void PoolManager::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		#ifdef LOG_INITIALIZATION
		LOG_TRACE("PoolManager initialized.");
		#endif
	}
	void PoolManager::Clear()
	{
		s_shaderPropertiesPoolMap.clear();
	}



	// Public methods:
	ShaderProperties* PoolManager::CheckOutShaderProperties(Shader* pShader)
	{
		return s_shaderPropertiesPoolMap[pShader].CheckOut(pShader);
	}
	StagingBuffer* PoolManager::CheckOutStagingBuffer(uint32_t size)
	{
		size = std::max(4096u,math::NextPowerOfTwo(size));
		return s_stagingBufferPoolMap[size].CheckOut(size);
	}
	void PoolManager::ReturnShaderProperties(Shader* pShader, ShaderProperties* pShaderProperties)
	{
		s_shaderPropertiesPoolMap[pShader].Return(pShaderProperties);
	}
	void PoolManager::ReturnStagingBuffer(uint32_t size, StagingBuffer* pStagingBuffer)
	{
		size = std::max(4096u, math::NextPowerOfTwo(size));
		s_stagingBufferPoolMap[size].Return(pStagingBuffer);
	}
}