#include "vulkanPreRenderCompute.h"
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
	PreRender::PreRender()
		: m_computeCallQueue(Context::GetFramesInFlight())
	{
		
	}
	PreRender::~PreRender()
	{
		if (!Context::IsDeviceIdle())
			Context::WaitDeviceIdle();
		ResetComputeCalls();
		RetireAllComputeCalls();
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
		if (!descriptorSetBindingHandle.Get())
			return nullptr;
		ComputeCall computeCall = { threadCount, ShaderHandle(*pComputeShader), descriptorSetBindingHandle, AccessMasks::None::none, AccessMasks::None::none };
		m_computeCallQueue.Add(computeCall);
		pComputeShader->AddPendingUse();
		return descriptorSetBindingHandle.Get();
	}
	void PreRender::RecordBarrier(emberBackendInterface::ComputeBarrierFlag srcBarrierFlags, emberBackendInterface::ComputeBarrierFlag dstBarrierFlags)
	{
		ComputeCall computeCall = { Uint3::zero, ShaderHandle(), DescriptorSetBindingHandle(), ComputeBarrierFlagsToVulkanAccessMask(srcBarrierFlags), ComputeBarrierFlagsToVulkanAccessMask(dstBarrierFlags) };
		m_computeCallQueue.Add(computeCall);
	}



	// Management:
	void PreRender::CommitComputeCalls(uint32_t frameIndex)
	{
		m_computeCallQueue.Commit(frameIndex);
	}
	void PreRender::RetireComputeCalls(uint32_t frameIndex)
	{
		m_computeCallQueue.Retire(frameIndex);
	}
	void PreRender::RetireAllComputeCalls()
	{
		m_computeCallQueue.RetireAll();
	}
	std::vector<ComputeCall>& PreRender::GetComputeCalls()
	{
		return m_computeCallQueue.GetPendingCalls();
	}
	void PreRender::ResetComputeCalls()
	{
		m_computeCallQueue.DiscardPending();
	}
	void PreRender::UpdateShaderData(uint32_t frameIndex)
	{
		m_computeCallQueue.UpdateShaderData(frameIndex);
	}
}