#include "vulkanPoolManager.h"
#include "descriptorSetMacros.h"
#include "emberMath.h"
#include "logger.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanShader.h"



namespace vulkanRendererBackend
{
	// Static members:
	bool PoolManager::s_isInitialized = false;
	std::unordered_map<Shader*, CallDescriptorSetBindingPool> PoolManager::s_callDescriptorSetBindingPoolMap;
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
		s_callDescriptorSetBindingPoolMap.clear();
		s_stagingBufferPoolMap.clear();
		s_isInitialized = false;
	}



	// Checkout:
	DescriptorSetBinding* PoolManager::CheckOutCallDescriptorSetBinding(Shader* pShader)
	{
		if (!IsValidCallDescriptorSetBindingShader(pShader))
			return nullptr;

		auto it = s_callDescriptorSetBindingPoolMap.find(pShader);
		if (it == s_callDescriptorSetBindingPoolMap.end())
			it = s_callDescriptorSetBindingPoolMap.try_emplace(pShader).first;
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
	void PoolManager::ReturnCallDescriptorSetBinding(Shader* pShader, DescriptorSetBinding* pDescriptorSetBinding)
	{
		if (pShader == nullptr)
		{
			LOG_ERROR("PoolManager::ReturnCallDescriptorSetBinding(...) failed. pShader is nullptr.");
			return;
		}

		auto it = s_callDescriptorSetBindingPoolMap.find(pShader);
		if (it == s_callDescriptorSetBindingPoolMap.end())
		{
			LOG_ERROR("PoolManager::ReturnCallDescriptorSetBinding(...) failed. No pool exists for shader '{}'.", pShader->GetName());
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
	void PoolManager::PrintCallDescriptorSetBindingPoolState()
	{
		for (auto& [pShader, callDescriptorSetBindingPool] : s_callDescriptorSetBindingPoolMap)
		{
			LOG_INFO("Shader '{}':", pShader->GetName());
			callDescriptorSetBindingPool.PrintPoolState();
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
	bool PoolManager::IsValidCallDescriptorSetBindingShader(const Shader* pShader)
	{
		if (pShader == nullptr)
		{
			LOG_ERROR("PoolManager::CheckOutCallDescriptorSetBinding(...) failed. pShader is nullptr.");
			return false;
		}

		const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts = pShader->GetVkDescriptorSetLayout();
		if (descriptorSetLayouts.size() <= CALL_SET_INDEX || descriptorSetLayouts[CALL_SET_INDEX] == VK_NULL_HANDLE)
		{
			LOG_ERROR("PoolManager::CheckOutCallDescriptorSetBinding(...) failed. Shader '{}' has no valid call descriptor set layout.", pShader->GetName());
			return false;
		}

		return true;
	}
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