#ifndef __INCLUDE_GUARD_computeSession_h__
#define __INCLUDE_GUARD_computeSession_h__
#include "emberMath.h"
#include <vector>
#include <vulkan/vulkan.h>

// Ember::TODO:
// -get VkFence from some pooling mechanism
// -get VkCommandBuffer from one VkCommandPool
// -create graphics folder and move graphics stuff there?
// -add other pools to poolManager, e.g. VkFence and VkCommandBuffer/VkCommandPool
// -allow for shaderProperties to be ceated with only one descriptorSet instead of framesInFlight many
// -allow for shaderProperties to be created empty (for pooling) and be initialized later.

namespace emberEngine
{
	namespace compute
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
			void Dispatch(VkCommandBuffer& commandBuffer, VkFence& fence);
			void ResetComputeCalls();
			std::vector<ComputeCall>& GetComputeCalls();
		};
	}
}



#endif // __INCLUDE_GUARD_computeSession_h__