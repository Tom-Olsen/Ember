#include "vulkanPreRenderCompute.h"
#include "logger.h"
#include "vulkanAccessMask.h"
#include "vulkanComputeCall.h"
#include "vulkanComputeShader.h"
#include "vulkanConvertAccessMask.h"
#include "vulkanPoolManager.h"
#include "vulkanDescriptorSetBinding.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	PreRender::PreRender()
	{
		m_callIndex = 0;
	}
	PreRender::~PreRender()
	{
		ResetComputeCalls();
	}



	// Movable:
	PreRender::PreRender(PreRender&& other) noexcept = default;
	PreRender& PreRender::operator=(PreRender&& other) noexcept = default;



	// Workload recording:
	void PreRender::RecordComputeShader(emberBackendInterface::IComputeShader* pIComputeShader, emberBackendInterface::IDescriptorSetBinding* pIDescriptorSetBinding, Uint3 threadCount)
	{
		if (!pIComputeShader)
		{
			LOG_ERROR("compute::PreRender::RecordComputeShader(...) failed. pIComputeShader is nullptr.");
			return;
		}
		if (!pIDescriptorSetBinding)
		{
			LOG_ERROR("compute::PreRender::RecordComputeShader(...) failed. pIDescriptorSetBinding is nullptr.");
			return;
		}
		if (threadCount[0] == 0 || threadCount[1] == 0 || threadCount[2] == 0)
		{
			LOG_ERROR("compute::PreRender::RecordComputeShader(...) failed. threadCount has 0 entry.");
			return;
		}

		// Setup compute call:
		ComputeCall computeCall = { m_callIndex, threadCount, static_cast<ComputeShader*>(pIComputeShader), static_cast<DescriptorSetBinding*>(pIDescriptorSetBinding), AccessMasks::None::none, AccessMasks::None::none };
		m_staticComputeCalls.push_back(computeCall);
		m_callIndex++;
	}
	emberBackendInterface::IDescriptorSetBinding* PreRender::RecordComputeShader(emberBackendInterface::IComputeShader* pIComputeShader, Uint3 threadCount)
	{
		if (!pIComputeShader)
		{
			LOG_ERROR("compute::PreRender::RecordComputeShader(...) failed. pIComputeShader is nullptr.");
			return nullptr;
		}
		if (threadCount[0] == 0 || threadCount[1] == 0 || threadCount[2] == 0)
		{
			LOG_ERROR("compute::PreRender::RecordComputeShader(...) failed. threadCount has 0 entry.");
			return nullptr;
		}

		DescriptorSetBinding* pDescriptorSetBinding = PoolManager::CheckOutDescriptorSetBinding(static_cast<Shader*>(static_cast<ComputeShader*>(pIComputeShader)));
		RecordComputeShader(pIComputeShader, static_cast<emberBackendInterface::IDescriptorSetBinding*>(pDescriptorSetBinding), threadCount);
		return pDescriptorSetBinding;
	}
	void PreRender::RecordBarrier(emberCommon::ComputeShaderAccessMask srcAccessMask, emberCommon::ComputeShaderAccessMask dstAccessMask)
	{
		ComputeCall computeCall = { m_callIndex, Uint3::zero, nullptr, nullptr, AccessMaskCommonToVulkan(srcAccessMask), AccessMaskCommonToVulkan(dstAccessMask) };
		m_staticComputeCalls.push_back(computeCall);
		m_callIndex++;
	}



	// Management:
	std::vector<ComputeCall*>& PreRender::GetComputeCallPointers()
	{
		// Populate draw call pointers vector according to callIndex:
		m_computeCallPointers.resize(m_staticComputeCalls.size() + m_dynamicComputeCalls.size());
		for (auto& computeCall : m_staticComputeCalls)
			m_computeCallPointers[computeCall.callIndex] = &computeCall;
		for (auto& computeCall : m_dynamicComputeCalls)
			m_computeCallPointers[computeCall.callIndex] = &computeCall;

		return m_computeCallPointers;
	}
	void PreRender::ResetComputeCalls()
	{
		// Return all pDescriptorSetBinding of compute calls back to the corresponding pool:
		for (ComputeCall& computeCall : m_dynamicComputeCalls)
			PoolManager::ReturnDescriptorSetBinding(computeCall.pComputeShader, computeCall.pDescriptorSetBinding);

		// Remove all computeCalls so next frame can start fresh:
		m_staticComputeCalls.clear();
		m_dynamicComputeCalls.clear();
		m_computeCallPointers.clear();
		m_callIndex = 0;
	}
}