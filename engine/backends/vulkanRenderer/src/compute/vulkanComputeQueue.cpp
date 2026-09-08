#include "vulkanComputeQueue.h"
#include "logger.h"
#include "vulkanAccessMask.h"
#include "vulkanComputeCall.h"
#include "vulkanComputeShader.h"
#include "vulkanContext.h"
#include "vulkanConvertComputeAccessMask.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanPoolManager.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	ComputeQueue::ComputeQueue()
		: m_computeCallQueue(Context::GetFramesInFlight())
	{
		
	}
	ComputeQueue::~ComputeQueue()
	{
		if (!Context::IsDeviceIdle())
			Context::WaitDeviceIdle();
		ResetComputeCalls();
		RetireAllComputeCalls();
	}



	// Movable:
	ComputeQueue::ComputeQueue(ComputeQueue&& other) noexcept = default;
	ComputeQueue& ComputeQueue::operator=(ComputeQueue&& other) noexcept = default;



	// Workload recording:
	emberBackendInterface::IDescriptorSetBinding* ComputeQueue::RecordComputeShader(emberBackendInterface::IComputeShader* pIComputeShader, Uint3 threadCount)
	{
		if (!pIComputeShader)
		{
			LOG_ERROR("compute::ComputeQueue::RecordComputeShader(...) failed. pIComputeShader is nullptr.");
			return nullptr;
		}
		if (threadCount[0] == 0 || threadCount[1] == 0 || threadCount[2] == 0)
		{
			LOG_ERROR("compute::ComputeQueue::RecordComputeShader(...) failed. threadCount has 0 entry.");
			return nullptr;
		}

		ComputeShader* pComputeShader = static_cast<ComputeShader*>(pIComputeShader);
		ComputeCall computeCall = { threadCount, ShaderHandle(*pComputeShader), DescriptorSetBindingHandle(), AccessMasks::None::none, AccessMasks::None::none };
		return RecordComputeCall(computeCall);
	}
	void ComputeQueue::RecordBarrier(emberBackendInterface::ComputeBarrierFlag srcBarrierFlags, emberBackendInterface::ComputeBarrierFlag dstBarrierFlags)
	{
		ComputeCall computeCall = { Uint3::zero, ShaderHandle(), DescriptorSetBindingHandle(), ComputeBarrierFlagsToVulkanAccessMask(srcBarrierFlags), ComputeBarrierFlagsToVulkanAccessMask(dstBarrierFlags) };
		m_computeCallQueue.Add(computeCall);
	}



	// Management:
	void ComputeQueue::CommitComputeCalls(uint32_t frameIndex)
	{
		m_computeCallQueue.Commit(frameIndex);
	}
	void ComputeQueue::RetireComputeCalls(uint32_t frameIndex)
	{
		m_computeCallQueue.Retire(frameIndex);
	}
	void ComputeQueue::RetireAllComputeCalls()
	{
		m_computeCallQueue.RetireAll();
	}
	std::vector<ComputeCall>& ComputeQueue::GetComputeCalls()
	{
		return m_computeCallQueue.GetPendingCalls();
	}
	void ComputeQueue::ResetComputeCalls()
	{
		m_computeCallQueue.DiscardPending();
	}
	void ComputeQueue::UpdateShaderData(uint32_t frameIndex)
	{
		m_computeCallQueue.UpdateShaderData(frameIndex);
	}



	// Protected methods:
	emberBackendInterface::IDescriptorSetBinding* ComputeQueue::RecordComputeCall(ComputeCall computeCall)
	{
		ComputeShader* pComputeShader = computeCall.GetComputeShader();
		DescriptorSetBindingHandle descriptorSetBindingHandle = PoolManager::CheckOutCallDescriptorSetBindingHandle(static_cast<Shader*>(pComputeShader));
		DescriptorSetBinding* pDescriptorSetBinding = descriptorSetBindingHandle.Get();
		if (!pDescriptorSetBinding)
			return nullptr;

		computeCall.callDescriptorSetBindingHandle = descriptorSetBindingHandle;
		m_computeCallQueue.Add(computeCall);
		pComputeShader->AddPendingUse();
		return pDescriptorSetBinding;
	}
}