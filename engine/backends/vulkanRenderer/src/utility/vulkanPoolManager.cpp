#include "vulkanPoolManager.h"
#include "emberMath.h"
#include "logger.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanShader.h"



namespace vulkanRendererBackend
{
	// Static members:
	bool PoolManager::s_isInitialized = false;
	std::unordered_map<Shader*, DescriptorSetBindingPool> PoolManager::s_descriptorSetBindingPoolMap;
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
		s_descriptorSetBindingPoolMap.clear();
		s_stagingBufferPoolMap.clear();
		s_isInitialized = false;
	}



	// Checkout:
	DescriptorSetBinding* PoolManager::CheckOutDescriptorSetBinding(Shader* pShader)
	{
		return s_descriptorSetBindingPoolMap[pShader].CheckOut(pShader);
	}
	StagingBuffer* PoolManager::CheckOutStagingBuffer(uint32_t size)
	{
		size = std::max(4096u, math::NextPowerOfTwo(size));
		return s_stagingBufferPoolMap[size].CheckOut(size);
	}



	// Return:
	void PoolManager::ReturnDescriptorSetBinding(Shader* pShader, DescriptorSetBinding* pDescriptorSetBinding)
	{
		s_descriptorSetBindingPoolMap[pShader].Return(pDescriptorSetBinding);
	}
	void PoolManager::ReturnStagingBuffer(uint32_t size, StagingBuffer* pStagingBuffer)
	{
		size = std::max(4096u, math::NextPowerOfTwo(size));
		s_stagingBufferPoolMap[size].Return(pStagingBuffer);
	}



	// Debugging:
	void PoolManager::PrintDescriptorSetBindingPoolState()
	{
		for (auto& [pShader, descriptorSetBindingPool] : s_descriptorSetBindingPoolMap)
		{
			LOG_INFO("Shader '{}':", pShader->GetName());
			descriptorSetBindingPool.PrintPoolState();
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