#include "vulkanPoolManager.h"
#include "emberMath.h"
#include "logger.h"
#include "vulkanShader.h"



namespace vulkanRendererBackend
{
	// Static members:
	bool PoolManager::s_isInitialized = false;
	std::unordered_map<Shader*, ShaderPropertiesPool> PoolManager::s_shaderPropertiesPoolMap;
	std::map<uint32_t, StagingBufferPool> PoolManager::s_stagingBufferPoolMap;



	// Public methods:
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



	// Checkout:
	ShaderProperties* PoolManager::CheckOutShaderProperties(Shader* pShader)
	{
		//return s_shaderPropertiesPoolMap[pShader].CheckOut(pShader);
		ShaderProperties* pShaderProperties = s_shaderPropertiesPoolMap[pShader].CheckOut(pShader);
		return pShaderProperties;
	}
	StagingBuffer* PoolManager::CheckOutStagingBuffer(uint32_t size)
	{
		size = std::max(4096u, math::NextPowerOfTwo(size));
		return s_stagingBufferPoolMap[size].CheckOut(size);
	}



	// Return:
	void PoolManager::ReturnShaderProperties(Shader* pShader, ShaderProperties* pShaderProperties)
	{
		s_shaderPropertiesPoolMap[pShader].Return(pShaderProperties);
	}
	void PoolManager::ReturnStagingBuffer(uint32_t size, StagingBuffer* pStagingBuffer)
	{
		size = std::max(4096u, math::NextPowerOfTwo(size));
		s_stagingBufferPoolMap[size].Return(pStagingBuffer);
	}



	// Debugging:
	void PoolManager::PrintShaderPropertiesPoolState()
	{
		for (auto& [pShader, shaderPropertiesPool] : s_shaderPropertiesPoolMap)
		{
			LOG_INFO("Shader '{}':", pShader->GetName());
			shaderPropertiesPool.PrintPoolState();
		}
	}
	void PoolManager::PrintStagingBufferPoolState()
	{
		for (auto& [bufferSize, stagingBufferPool] : s_stagingBufferPoolMap)
		{
			LOG_INFO("BufferSize '{}':", bufferSize);
			stagingBufferPool.PrintPoolState();
		}
	}
}