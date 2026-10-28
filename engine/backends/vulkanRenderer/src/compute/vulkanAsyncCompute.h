#pragma once
#include "iCompute.h"
#include "vulkanComputeQueue.h"
#include "vulkanRendererExport.h"
#include <memory>
#include <queue>
#include <vector>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class CommandPool;



	class VULKAN_RENDERER_API Async : public emberBackendInterface::ICompute::IAsync
	{
	private: // Structs:
		struct Session
		{
		public: // Enums:
			enum class State { idle, running, recording };

		public: // Members:
			ComputeQueue computeQueue;
			State state = State::idle;
			uint32_t queueIndex = UINT32_MAX;
			uint64_t completionValue = 0;
		};

	private: // Members:
		uint32_t m_nextQueueIndex = 0;
		std::vector<CommandPool> m_pCommandPools;
		std::vector<std::unique_ptr<Session>> m_computeSessions;
		std::vector<uint32_t> m_pendingSessionCounts;
		std::queue<uint32_t> m_freeIndices;

	public: // Methods:
		// Constructor/Destructor:
		Async();
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
		void CollectFinishedSessions();

		// Workload recording:
		emberBackendInterface::ICompute::IQueue& GetRecordingQueue(uint32_t sessionID) override;

		// Debugging:
		void PrintSessionsState();

	private: // Methods:
		// Cleanup:
		void Cleanup();
		void MoveFrom(Async& other) noexcept;

		// Helpers:
		void AddSession();
		uint32_t SelectQueue();
		void ResetComputeSession(uint32_t sessionID);
	};
}