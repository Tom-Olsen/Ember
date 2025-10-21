#include "vulkanPoolManager.h"



namespace vulkanRendererBackend
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
	}
	void PoolManager::Clear()
	{
		s_shaderPropertiesPoolMap.clear();
		s_stagingBufferPoolMap.clear();
		s_isInitialized = false;
	}



	// Public methods:
	ShaderProperties* PoolManager::CheckOutShaderProperties(Shader* pShader)
	{
		return s_shaderPropertiesPoolMap[pShader].CheckOut(pShader);
	}
	StagingBuffer* PoolManager::CheckOutStagingBuffer(uint32_t size)
	{
		size = std::max(4096u, NextPowerOfTwo(size));
		return s_stagingBufferPoolMap[size].CheckOut(size);
	}
	void PoolManager::ReturnShaderProperties(Shader* pShader, ShaderProperties* pShaderProperties)
	{
		s_shaderPropertiesPoolMap[pShader].Return(pShaderProperties);
	}
	void PoolManager::ReturnStagingBuffer(uint32_t size, StagingBuffer* pStagingBuffer)
	{
		size = std::max(4096u, NextPowerOfTwo(size));
		s_stagingBufferPoolMap[size].Return(pStagingBuffer);
	}



	// Private methods:
	uint32_t PoolManager::NextPowerOfTwo(uint32_t n)
	{
		if (n == 0) return 1;
		n--;
		n |= n >> 1;
		n |= n >> 2;
		n |= n >> 4;
		n |= n >> 8;
		n |= n >> 16;
		n++;
		return n;
	}
}