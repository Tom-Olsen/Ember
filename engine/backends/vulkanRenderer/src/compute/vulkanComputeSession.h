#pragma once
#include "vulkanComputeCall.h"
#include <vector>



// Forward declaration:
typedef struct VkCommandBuffer_T* VkCommandBuffer;



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct DeviceQueue;



	struct ComputeSession
	{
	public: // Enums:
		enum class State { idle, running, recording };

	private: // Members:
		std::vector<ComputeCall> m_computeCalls;

	public: // Members:
		State state = State::idle;
		uint32_t queueIndex = UINT32_MAX;	// which compute queue this session runs on.
		uint64_t completionValue = 0;		// for timeline semaphore completion check.

	public: // Methods:
		void RecordComputeCall(const ComputeCall& computeCall);
		uint64_t Dispatch(VkCommandBuffer commandBuffer, const DeviceQueue& queue, float time, float deltaTime);
		void ResetComputeCalls();
		std::vector<ComputeCall>& GetComputeCalls();
	};
}