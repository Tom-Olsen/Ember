#pragma once
#include "vulkanComputeCall.h"
#include <cstdint>
#include <vector>



namespace vulkanRendererBackend
{
	class ComputeCallQueue
	{
	private: // Members:
		std::vector<ComputeCall> m_pendingCalls;
		std::vector<std::vector<ComputeCall>> m_submittedCalls;

	public: // Methods:
		// Constructor:
		explicit ComputeCallQueue(uint32_t framesInFlight);

		// Queue management:
		void Add(const ComputeCall& computeCall);
		void Commit(uint32_t frameIndex);
		void Retire(uint32_t frameIndex);
		void DiscardPending();
		void RetireAll();
		void UpdateShaderData(uint32_t frameIndex);

		// Getters:
		std::vector<ComputeCall>& GetPendingCalls();
		const std::vector<ComputeCall>& GetPendingCalls() const;

	private: // Methods:
		void Release(std::vector<ComputeCall>& computeCalls);
	};
}