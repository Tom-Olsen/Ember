#include "vulkanPreRenderCompute.h"
#include "logger.h"
#include "vulkanAccessMask.h"
#include "vulkanComputeCall.h"
#include "vulkanComputeShader.h"
#include "vulkanConvertComputeAccessMask.h"
#include "vulkanContext.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanPoolManager.h"
#include <assert.h>
#include <utility>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	PreRender::PreRender()
	{
		m_submittedComputeCalls.resize(Context::GetFramesInFlight());
	}
	PreRender::~PreRender()
	{
		if (!Context::IsDeviceIdle())
			Context::WaitDeviceIdle();
		ResetComputeCalls();
		CompleteAllComputeCalls();
	}



	// Movable:
	PreRender::PreRender(PreRender&& other) noexcept = default;
	PreRender& PreRender::operator=(PreRender&& other) noexcept = default;



	// Workload recording:
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

		ComputeShader* pComputeShader = static_cast<ComputeShader*>(pIComputeShader);
		DescriptorSetBindingHandle descriptorSetBindingHandle = PoolManager::CheckOutCallDescriptorSetBindingHandle(static_cast<Shader*>(pComputeShader));
		ComputeCall computeCall = { threadCount, ShaderHandle(*pComputeShader), descriptorSetBindingHandle, AccessMasks::None::none, AccessMasks::None::none };
		m_computeCalls.push_back(computeCall);
		pComputeShader->AddPendingUse();
		return descriptorSetBindingHandle.Get();
	}
	void PreRender::RecordBarrier(emberBackendInterface::ComputeBarrierFlag srcBarrierFlags, emberBackendInterface::ComputeBarrierFlag dstBarrierFlags)
	{
		ComputeCall computeCall = { Uint3::zero, ShaderHandle(), DescriptorSetBindingHandle(), ComputeBarrierFlagsToVulkanAccessMask(srcBarrierFlags), ComputeBarrierFlagsToVulkanAccessMask(dstBarrierFlags) };
		m_computeCalls.push_back(computeCall);
	}



	// Management:
	void PreRender::CommitComputeCalls(uint32_t frameIndex)
	{
		assert(frameIndex < m_submittedComputeCalls.size());
		assert(m_submittedComputeCalls[frameIndex].empty());
		std::swap(m_submittedComputeCalls[frameIndex], m_computeCalls);
	}
	void PreRender::CompleteComputeCalls(uint32_t frameIndex)
	{
		assert(frameIndex < m_submittedComputeCalls.size());
		ReleaseComputeCalls(m_submittedComputeCalls[frameIndex]);
	}
	void PreRender::CompleteAllComputeCalls()
	{
		for (std::vector<ComputeCall>& computeCalls : m_submittedComputeCalls)
			ReleaseComputeCalls(computeCalls);
	}
	std::vector<ComputeCall>& PreRender::GetComputeCalls()
	{
		return m_computeCalls;
	}
	void PreRender::ResetComputeCalls()
	{
		ReleaseComputeCalls(m_computeCalls);
	}



	// Private methods:
	void PreRender::ReleaseComputeCalls(std::vector<ComputeCall>& computeCalls)
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