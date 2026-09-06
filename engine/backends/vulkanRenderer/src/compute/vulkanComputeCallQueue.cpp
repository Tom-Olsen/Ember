#include "vulkanComputeCallQueue.h"
#include "vulkanComputeShader.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanPoolManager.h"
#include <cassert>
#include <stdexcept>
#include <utility>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor:
	ComputeCallQueue::ComputeCallQueue(uint32_t framesInFlight)
	{
		if (framesInFlight == 0)
			throw std::invalid_argument("ComputeCallQueue constructor failed. framesInFlight must be greater than 0.");
		m_submittedCalls.resize(framesInFlight);
	}



	// Queue management:
	void ComputeCallQueue::Add(const ComputeCall& computeCall)
	{
		m_pendingCalls.push_back(computeCall);
	}
	void ComputeCallQueue::Commit(uint32_t frameIndex)
	{
		assert(frameIndex < m_submittedCalls.size());
		assert(m_submittedCalls[frameIndex].empty());
		std::swap(m_submittedCalls[frameIndex], m_pendingCalls);
	}
	void ComputeCallQueue::Retire(uint32_t frameIndex)
	{
		assert(frameIndex < m_submittedCalls.size());
		Release(m_submittedCalls[frameIndex]);
	}
	void ComputeCallQueue::DiscardPending()
	{
		Release(m_pendingCalls);
	}
	void ComputeCallQueue::RetireAll()
	{
		for (std::vector<ComputeCall>& computeCalls : m_submittedCalls)
			Release(computeCalls);
	}
	void ComputeCallQueue::UpdateShaderData(uint32_t frameIndex)
	{
		for (ComputeCall& computeCall : m_pendingCalls)
		{
			if (computeCall.IsBarrier())
				continue;
			computeCall.GetComputeShader()->GetDescriptorSetBinding()->UpdateShaderData(frameIndex);
			computeCall.callDescriptorSetBindingHandle.Get()->UpdateShaderData(frameIndex);
		}
	}



	// Getters:
	std::vector<ComputeCall>& ComputeCallQueue::GetPendingCalls()
	{
		return m_pendingCalls;
	}
	const std::vector<ComputeCall>& ComputeCallQueue::GetPendingCalls() const
	{
		return m_pendingCalls;
	}



	// Private methods:
	void ComputeCallQueue::Release(std::vector<ComputeCall>& computeCalls)
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