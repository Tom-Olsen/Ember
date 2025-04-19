#include "poolManager.h"



namespace emberEngine
{
	// Static members:
	bool PoolManager::s_isInitialized = false;
	std::unordered_map<ComputeShader*, ShaderPropertiesPool> PoolManager::s_shaderPropertiesPoolMap;



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
	}



	// Public methods:
	ShaderProperties* PoolManager::CheckOutShaderProperties(ComputeShader* pComputeShader)
	{
		return s_shaderPropertiesPoolMap[pComputeShader].CheckOut((Shader*)pComputeShader);
	}
	void PoolManager::ReturnShaderProperties(ComputeShader* pComputeShader, ShaderProperties* pShaderProperties)
	{
		s_shaderPropertiesPoolMap[pComputeShader].Return(pShaderProperties);
	}
}