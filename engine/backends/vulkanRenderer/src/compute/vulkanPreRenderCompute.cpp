#include "vulkanPreRenderCompute.h"
#include "logger.h"
#include "vulkanAccessMask.h"
#include "vulkanComputeCall.h"
#include "vulkanComputeShader.h"
#include "vulkanConvertComputeAccessMask.h"
#include "vulkanPoolManager.h"
#include "vulkanDescriptorSetBinding.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	PreRender::PreRender()
	{

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
		// Record static compute call.
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
		ComputeCall computeCall = { threadCount, static_cast<ComputeShader*>(pIComputeShader), static_cast<DescriptorSetBinding*>(pIDescriptorSetBinding), false, AccessMasks::None::none, AccessMasks::None::none };
		m_computeCalls.push_back(computeCall);
	}
	emberBackendInterface::IDescriptorSetBinding* PreRender::RecordComputeShader(emberBackendInterface::IComputeShader* pIComputeShader, Uint3 threadCount)
	{
		// Record dynamic compute call.
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

		DescriptorSetBinding* pDescriptorSetBinding = PoolManager::CheckOutCallDescriptorSetBinding(static_cast<Shader*>(static_cast<ComputeShader*>(pIComputeShader)));
		ComputeCall computeCall = { threadCount, static_cast<ComputeShader*>(pIComputeShader), pDescriptorSetBinding, true, AccessMasks::None::none, AccessMasks::None::none };
		m_computeCalls.push_back(computeCall);
		return pDescriptorSetBinding;
	}
	void PreRender::RecordBarrier(emberBackendInterface::ComputeBarrierFlag srcBarrierFlags, emberBackendInterface::ComputeBarrierFlag dstBarrierFlags)
	{
		ComputeCall computeCall = { Uint3::zero, nullptr, nullptr, false, ComputeBarrierFlagsToVulkanAccessMask(srcBarrierFlags), ComputeBarrierFlagsToVulkanAccessMask(dstBarrierFlags) };
		m_computeCalls.push_back(computeCall);
	}



	// Management:
	std::vector<ComputeCall>& PreRender::GetComputeCalls()
	{
		return m_computeCalls;
	}
	void PreRender::ResetComputeCalls()
	{
		// Return all owned pDescriptorSetBinding of compute calls back to the corresponding pool:
		for (ComputeCall& computeCall : m_computeCalls)
		{
			if (computeCall.ownsDescriptorSetBinding && computeCall.pComputeShader && computeCall.pDescriptorSetBinding)
				PoolManager::ReturnCallDescriptorSetBinding(computeCall.pComputeShader, computeCall.pDescriptorSetBinding);
		}

		// Remove all computeCalls so next frame can start fresh:
		m_computeCalls.clear();
	}
}