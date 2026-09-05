#include "vulkanRenderCompute.h"
#include "logger.h"
#include "vulkanAccessMask.h"
#include "vulkanComputeCall.h"
#include "vulkanComputeShader.h"
#include "vulkanConvertComputeAccessMask.h"
#include "vulkanContext.h"
#include "vulkanConvertComputeAccessMask.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanPoolManager.h"
#include <assert.h>
#include <utility>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	Render::Render()
	{
		m_submittedComputeCalls.resize(Context::GetFramesInFlight());
	}
	Render::~Render()
	{
		if (!Context::IsDeviceIdle())
			Context::WaitDeviceIdle();
		ResetComputeCalls();
		CompleteAllComputeCalls();
	}



	// Movable:
	Render::Render(Render&& other) noexcept = default;
	Render& Render::operator=(Render&& other) noexcept = default;



	// Workload recording:
	emberBackendInterface::IDescriptorSetBinding* Render::RecordComputeShader(emberBackendInterface::IComputeShader* pIComputeShader, Uint3 threadCount)
	{
		// Record dynamic compute call.
		if (!pIComputeShader)
		{
			LOG_ERROR("compute::Render::RecordComputeShader(...) failed. pIComputeShader is nullptr.");
			return nullptr;
		}
		if (threadCount[0] == 0 || threadCount[1] == 0 || threadCount[2] == 0)
		{
			LOG_ERROR("compute::Render::RecordComputeShader(...) failed. threadCount has 0 entry.");
			return nullptr;
		}

		ComputeShader* pComputeShader = static_cast<ComputeShader*>(pIComputeShader);
		DescriptorSetBindingHandle descriptorSetBindingHandle = PoolManager::CheckOutCallDescriptorSetBindingHandle(static_cast<Shader*>(pComputeShader));
		ComputeCall computeCall = { threadCount, ShaderHandle(*pComputeShader), descriptorSetBindingHandle, AccessMasks::None::none, AccessMasks::None::none };
		m_computeCalls.push_back(computeCall);
		pComputeShader->AddPendingUse();
		return descriptorSetBindingHandle.Get();
	}
	void Render::RecordBarrier(emberBackendInterface::ComputeBarrierFlag srcBarrierFlags, emberBackendInterface::ComputeBarrierFlag dstBarrierFlags)
	{
		ComputeCall computeCall = { Uint3::zero, ShaderHandle(), DescriptorSetBindingHandle(), ComputeBarrierFlagsToVulkanAccessMask(srcBarrierFlags), ComputeBarrierFlagsToVulkanAccessMask(dstBarrierFlags) };
		m_computeCalls.push_back(computeCall);
	}



	// Management:
	void Render::CommitComputeCalls(uint32_t frameIndex)
	{
		assert(frameIndex < m_submittedComputeCalls.size());
		assert(m_submittedComputeCalls[frameIndex].empty());
		std::swap(m_submittedComputeCalls[frameIndex], m_computeCalls);
	}
	void Render::CompleteComputeCalls(uint32_t frameIndex)
	{
		assert(frameIndex < m_submittedComputeCalls.size());
		ReleaseComputeCalls(m_submittedComputeCalls[frameIndex]);
	}
	void Render::CompleteAllComputeCalls()
	{
		for (std::vector<ComputeCall>& computeCalls : m_submittedComputeCalls)
			ReleaseComputeCalls(computeCalls);
	}
	std::vector<ComputeCall>& Render::GetComputeCalls()
	{
		return m_computeCalls;
	}
	void Render::ResetComputeCalls()
	{
		ReleaseComputeCalls(m_computeCalls);
	}



	// Private methods:
	void Render::ReleaseComputeCalls(std::vector<ComputeCall>& computeCalls)
	{
		for (ComputeCall& computeCall : computeCalls)
		{
			PoolManager::ReturnCallDescriptorSetBinding(computeCall.callDescriptorSetBindingHandle);
			if (!computeCall.IsBarrier())
				computeCall.GetComputeShader()->RemovePendingUse();
		}
		computeCalls.clear();
	}
}