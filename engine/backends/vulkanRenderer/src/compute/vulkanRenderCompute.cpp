#include "vulkanRenderCompute.h"
#include "logger.h"
#include "vulkanAccessMask.h"
#include "vulkanComputeCall.h"
#include "vulkanComputeShader.h"
#include "vulkanContext.h"
#include "vulkanConvertComputeAccessMask.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanPoolManager.h"



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	Render::Render()
		: m_computeCallQueue(Context::GetFramesInFlight())
	{
		
	}
	Render::~Render()
	{
		if (!Context::IsDeviceIdle())
			Context::WaitDeviceIdle();
		ResetComputeCalls();
		RetireAllComputeCalls();
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
		m_computeCallQueue.Add(computeCall);
		pComputeShader->AddPendingUse();
		return descriptorSetBindingHandle.Get();
	}
	void Render::RecordBarrier(emberBackendInterface::ComputeBarrierFlag srcBarrierFlags, emberBackendInterface::ComputeBarrierFlag dstBarrierFlags)
	{
		ComputeCall computeCall = { Uint3::zero, ShaderHandle(), DescriptorSetBindingHandle(), ComputeBarrierFlagsToVulkanAccessMask(srcBarrierFlags), ComputeBarrierFlagsToVulkanAccessMask(dstBarrierFlags) };
		m_computeCallQueue.Add(computeCall);
	}



	// Management:
	void Render::CommitComputeCalls(uint32_t frameIndex)
	{
		m_computeCallQueue.Commit(frameIndex);
	}
	void Render::RetireComputeCalls(uint32_t frameIndex)
	{
		m_computeCallQueue.Retire(frameIndex);
	}
	void Render::RetireAllComputeCalls()
	{
		m_computeCallQueue.RetireAll();
	}
	std::vector<ComputeCall>& Render::GetComputeCalls()
	{
		return m_computeCallQueue.GetPendingCalls();
	}
	void Render::ResetComputeCalls()
	{
		m_computeCallQueue.DiscardPending();
	}
	void Render::UpdateShaderData(uint32_t frameIndex)
	{
		m_computeCallQueue.UpdateShaderData(frameIndex);
	}
}