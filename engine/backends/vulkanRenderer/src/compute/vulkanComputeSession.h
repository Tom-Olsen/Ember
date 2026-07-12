#pragma once
#include "vulkanComputeCall.h"
#include <vector>



// Forward declaration:
typedef struct VkCommandBuffer_T* VkCommandBuffer;
typedef struct VkFence_T* VkFence;



namespace vulkanRendererBackend
{
	struct ComputeSession
	{
	public: // Members:
		enum class State { idle, running, recording };
		State state = State::idle;

	private: // Members:
		std::vector<ComputeCall> m_computeCalls;

	public: // Methods:
		void RecordComputeCall(const ComputeCall& computeCall);
		void Dispatch(VkCommandBuffer commandBuffer, VkFence fence, float time, float deltaTime);
		void ResetComputeCalls();
		std::vector<ComputeCall>& GetComputeCalls();
	};
}