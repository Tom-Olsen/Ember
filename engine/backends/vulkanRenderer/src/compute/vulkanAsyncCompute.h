#pragma once
#include "iCompute.h"
#include "commonComputeAccessMask.h"
#include "emberMath.h"
#include "vulkanAccessMask.h"
#include "vulkanRendererExport.h"
#include <queue>
#include <vector>



// Forward declaration:
typedef struct VkFence_T* VkFence;
namespace emberBackendInterface
{
	class IComputeShader;
	class IDescriptorSetBinding;
}



namespace vulkanRendererBackend
{
	// Forward declarations:
	class CommandPool;


	// Forward declarations:
	class ComputeCall;
	struct ComputeSession;



	class VULKAN_RENDERER_API Async : public emberBackendInterface::ICompute::IAsync
	{
	private: // Members:
		uint32_t m_sessionCount;
		std::vector<CommandPool> m_pCommandPools;
		std::vector<ComputeSession> m_computeSessions;
		std::vector<VkFence> m_fences;
		std::queue<uint32_t> m_freeIndices;

	public: // Methods:
		// Constructor/Destructor:
		Async(uint32_t sessionCount);
		~Async();

		// Non-copyable:
		Async(const Async&) = delete;
		Async& operator=(const Async&) = delete;

		// Movable:
		Async(Async&& other) noexcept;
		Async& operator=(Async&& other) noexcept;

		// Dispatch logic:
		uint32_t CreateComputeSession() override;
		void DispatchComputeSession(uint32_t sessionID, float time, float deltaTime) override;
		bool IsFinished(uint32_t sessionID) override;
		void WaitForFinish(uint32_t sessionID) override;

		// Workload recording:
		void RecordComputeShader(uint32_t sessionID, emberBackendInterface::IComputeShader* pIComputeShader, emberBackendInterface::IDescriptorSetBinding* pIDescriptorSetBinding, Uint3 threadCount) override;
		emberBackendInterface::IDescriptorSetBinding* RecordComputeShader(uint32_t sessionID, emberBackendInterface::IComputeShader* pIComputeShader, Uint3 threadCount) override;
		void RecordBarrier(uint32_t sessionID, emberCommon::ComputeAccessFlag srcAccessMask, emberCommon::ComputeAccessFlag dstAccessMask) override;

		// Debugging:
		void PrintSessionsState();

	private: // Methods:
		void ResetComputeSession(uint32_t sessionID);
	};
}