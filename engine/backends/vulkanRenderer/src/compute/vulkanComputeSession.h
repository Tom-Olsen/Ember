#pragma once
#include <vector>
#include <vulkan/vulkan.h>



// vulkanRendererBackend::ToDo: (these are older, maybe some are debrecated?)
// -get VkFence from some pooling mechanism
// -get VkCommandBuffer from one VkCommandPool
// -add other pools to poolManager, e.g. VkFence and VkCommandBuffer/VkCommandPool
// -allow for shaderProperties to be ceated with only one descriptorSet instead of framesInFlight many
// -allow for shaderProperties to be created empty (for pooling) and be initialized later.



namespace vulkanRendererBackend
{
	// Forward decleration:
	struct ComputeCall;



	struct ComputeSession
	{
	public: // Members:
		enum State { idle, running, recording };
		State state = State::idle;

	private: // Members:
		std::vector<ComputeCall> m_computeCalls;

	public: // Methods:
		void RecordComputeCall(const ComputeCall& computeCall);
		void Dispatch(VkCommandBuffer& commandBuffer, VkFence& fence, float time, float deltaTime);
		void ResetComputeCalls();
		std::vector<ComputeCall>& GetComputeCalls();
	};
}