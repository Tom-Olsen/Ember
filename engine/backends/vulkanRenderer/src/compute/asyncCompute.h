#ifndef __INCLUDE_GUARD_vulkanRendererBackend_asyncCompute_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_asyncCompute_h__
#include "emberMath.h"
#include <queue>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class ComputeShader;
	class ShaderProperties;
	class CommandPool;


	// Forward declarations:
	class ComputeCall;
	struct ComputeSession;



	class Async
	{
	private: // Members
		static bool s_isInitialized;
		static int s_sessionCount;
		static std::vector<CommandPool> s_pCommandPools;
		static std::vector<ComputeSession> s_computeSessions;
		static std::vector<VkFence> s_fences;
		static std::queue<uint32_t> s_freeIndices;

	public: // Methods
		static void Init(int sessionCount);
		static void Clear();

		// Dispatch logic:
		static uint32_t CreateComputeSession();
		static void DispatchComputeSession(uint32_t sessionID, float time, float deltaTime);
		static bool IsFinished(uint32_t sessionID);
		static void WaitForFinish(uint32_t sessionID);

		// Workload recording:
		static ShaderProperties* RecordComputeShader(uint32_t sessionID, ComputeShader* pComputeShader, Uint3 threadCount);
		static void RecordComputeShader(uint32_t sessionID, ComputeShader* pComputeShader, ShaderProperties* pShaderProperties, Uint3 threadCount);
		static void RecordBarrier(uint32_t sessionID, VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask);

	private: // Methods
		static void ResetComputeSession(uint32_t sessionID);

		// Delete all constructors:
		Async() = delete;
		Async(const Async&) = delete;
		Async& operator=(const Async&) = delete;
		~Async() = delete;
	};
}



#endif // __INCLUDE_GUARD_vulkanRendererBackend_asyncCompute_h__