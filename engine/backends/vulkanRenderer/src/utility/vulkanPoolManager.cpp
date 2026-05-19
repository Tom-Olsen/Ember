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
		auto it = s_descriptorSetBindingPoolMap.find(pShader);
		if (it == s_descriptorSetBindingPoolMap.end())
			it = s_descriptorSetBindingPoolMap.try_emplace(pShader).first;
		return it->second.CheckOut(pShader);
	}
	StagingBuffer* PoolManager::CheckOutStagingBuffer(uint32_t size)
	{
		size = std::max(4096u, math::NextPowerOfTwo(size));
		auto it = s_stagingBufferPoolMap.find(size);
		if (it == s_stagingBufferPoolMap.end())
			it = s_stagingBufferPoolMap.try_emplace(size, GetAvailableStorageBufferCount(size)).first;
		return it->second.CheckOut(size);
	}



	// Return:
	void PoolManager::ReturnDescriptorSetBinding(Shader* pShader, DescriptorSetBinding* pDescriptorSetBinding)
	{
		if (pShader == nullptr)
		{
			LOG_ERROR("PoolManager::ReturnDescriptorSetBinding(...) failed. pShader is nullptr.");
			return;
		}

		auto it = s_descriptorSetBindingPoolMap.find(pShader);
		if (it == s_descriptorSetBindingPoolMap.end())
		{
			LOG_ERROR("PoolManager::ReturnDescriptorSetBinding(...) failed. No pool exists for shader '{}'.", pShader->GetName());
			return;
		}
		it->second.Return(pDescriptorSetBinding);
	}
	void PoolManager::ReturnStagingBuffer(uint32_t size, StagingBuffer* pStagingBuffer)
	{
		size = std::max(4096u, math::NextPowerOfTwo(size));
		auto it = s_stagingBufferPoolMap.find(size);
		if (it == s_stagingBufferPoolMap.end())
		{
			LOG_ERROR("PoolManager::ReturnStagingBuffer(...) failed. No pool exists for buffer size '{}'.", size);
			return;
		}
		it->second.Return(pStagingBuffer);
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



	// Private methods:
	uint16_t PoolManager::GetAvailableStorageBufferCount(uint32_t size)
	{
		if (size <= 4u * 1024u)             //  0KiB-  4KiB,    3
			return 64;
		if (size <= 64u * 1024u)            //  4KiB- 64KiB,    5
			return 32;
		if (size <= 1024u * 1024u)          // 64KiB-  1MiB,    5
			return 16;
		if (size <= 8u * 1024u * 1024u)     //  1MiB-  8MiB,    4
			return 8;
		if (size <= 64u * 1024u * 1024u)    //  8MiB- 64MiB,    4
			return 4;
		if (size <= 256u * 1024u * 1024u)   // 64MiB-256MiB,    3
			return 1;
		return 0;
	}
}