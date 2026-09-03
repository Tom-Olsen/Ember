#include "vulkanPostRenderCompute.h"
#include "emberMath.h"
#include "logger.h"
#include "vulkanAccessMask.h"
#include "vulkanComputeCall.h"
#include "vulkanComputeShader.h"
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
	PostRender::PostRender()
		: m_postProcessingCallCount(0)
	{
		m_submittedComputeCalls.resize(Context::GetFramesInFlight());
	}
	PostRender::~PostRender()
	{
		if (!Context::IsDeviceIdle())
			Context::WaitDeviceIdle();
		ResetComputeCalls();
		CompleteAllComputeCalls();
	}



	// Movable:
	PostRender::PostRender(PostRender&& other) noexcept = default;
	PostRender& PostRender::operator=(PostRender&& other) noexcept = default;



	// Workload recording:
	emberBackendInterface::IDescriptorSetBinding* PostRender::RecordComputeShader(emberBackendInterface::IComputeShader* pIComputeShader)
	{
		return RecordComputeShader(pIComputeShader, false);
	}
	emberBackendInterface::IDescriptorSetBinding* PostRender::RecordPostProcessingShader(emberBackendInterface::IComputeShader* pIComputeShader)
	{
		return RecordComputeShader(pIComputeShader, true);
	}



	// Management:
	void PostRender::CommitComputeCalls(uint32_t frameIndex)
	{
		assert(frameIndex < m_submittedComputeCalls.size());
		assert(m_submittedComputeCalls[frameIndex].empty());
		std::swap(m_submittedComputeCalls[frameIndex], m_computeCalls);
		m_postProcessingCallCount = 0;
	}
	void PostRender::CompleteAllComputeCalls()
	{
		for (std::vector<ComputeCall>& computeCalls : m_submittedComputeCalls)
			ReleaseComputeCalls(computeCalls);
	}
	void PostRender::CompleteComputeCalls(uint32_t frameIndex)
	{
		assert(frameIndex < m_submittedComputeCalls.size());
		ReleaseComputeCalls(m_submittedComputeCalls[frameIndex]);
	}
	std::vector<ComputeCall>& PostRender::GetComputeCalls()
	{
		return m_computeCalls;
	}
	size_t PostRender::GetPostProcessingCallCount() const
	{
		return m_postProcessingCallCount;
	}
	void PostRender::ResetComputeCalls()
	{
		ReleaseComputeCalls(m_computeCalls);
		m_postProcessingCallCount = 0;
	}



	// Private methods:
	emberBackendInterface::IDescriptorSetBinding* PostRender::RecordComputeShader(emberBackendInterface::IComputeShader* pIComputeShader, bool isPostProcessing)
	{
		// Record dynamic compute call.
		if (!pIComputeShader)
		{
			LOG_ERROR("compute::PostRender::RecordComputeShader(...) failed. pIComputeShader is nullptr.");
			return nullptr;
		}

		// The renderer resolves the thread count from the current scene-color extent while preparing the frame.
		// This keeps queued calls independent of render-pass ownership and automatically follows render-resolution changes.
		Uint3 threadCount = Uint3::zero;
		ComputeShader* pComputeShader = static_cast<ComputeShader*>(pIComputeShader);
		DescriptorSetBindingHandle descriptorSetBindingHandle = PoolManager::CheckOutCallDescriptorSetBindingHandle(static_cast<Shader*>(pComputeShader));
		DescriptorSetBinding* pDescriptorSetBinding = descriptorSetBindingHandle.Get();
		if (!pDescriptorSetBinding)
			return nullptr;
		if (isPostProcessing && (!pDescriptorSetBinding->HasBinding("inputImage") || !pDescriptorSetBinding->HasBinding("outputImage")))
		{
			PoolManager::ReturnCallDescriptorSetBinding(descriptorSetBindingHandle);
			LOG_ERROR("compute::PostRender::RecordPostProcessingShader(...) failed. Compute shader '{}' must declare CALL_SET storage images named 'inputImage' and 'outputImage'.", pComputeShader->GetDebugName());
			return nullptr;
		}

		ComputeCall computeCall = { threadCount, ShaderHandle(*pComputeShader), descriptorSetBindingHandle, AccessMasks::None::none, AccessMasks::None::none, isPostProcessing };
		m_computeCalls.push_back(computeCall);
		pComputeShader->AddPendingUse();
		if (isPostProcessing)
			m_postProcessingCallCount++;
		return pDescriptorSetBinding;
	}
	void PostRender::ReleaseComputeCalls(std::vector<ComputeCall>& computeCalls)
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