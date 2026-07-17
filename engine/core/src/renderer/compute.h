#pragma once
#include "iCompute.h"
#include "commonRendererCreateInfo.h"
#include "emberCoreExport.h"
#include "emberMath.h"
#include <array>
#include <memory>
#include <vector>



namespace emberCore
{
	// Forward decleration:
	class ComputeShader;
	class Renderer;
	class ShaderProperties;



	// Allows for easier delegation to correct compute queue:
	enum class ComputeType
	{
		async,
		postRender,
		preRender,
		physics
	};



	class EMBER_CORE_API Compute
	{
	public: // Subclasses:
		class EMBER_CORE_API Async
		{
		private: // Members:
			static emberBackendInterface::ICompute::IAsync* s_pIAsync;

		public: // Methods:
			// Constructor/Destructor:
			static void Init(emberBackendInterface::ICompute::IAsync* pIAsync);
			static void Clear();

			// Dispatch logic:
			static uint32_t CreateComputeSession();
			static void DispatchComputeSession(uint32_t sessionID);
			static void DispatchComputeSessionAndWait(uint32_t sessionID);
			static bool IsFinished(uint32_t sessionID);
			static void WaitForFinish(uint32_t sessionID);

			// Workload recording:
			static ShaderProperties RecordComputeShader(uint32_t sessionID, ComputeShader& computeShader, Uint3 threadCount);
			static void RecordBarrier(uint32_t sessionID, emberBackendInterface::ComputeBarrierFlag srcBarrierFlags, emberBackendInterface::ComputeBarrierFlag dstBarrierFlags);
			static void RecordBarrierWaitShaderWriteBeforeRead(uint32_t sessionID);
			static void RecordBarrierWaitStorageWriteBeforeRead(uint32_t sessionID);
			static void RecordBarrierWaitStorageWriteBeforeWrite(uint32_t sessionID);
			static void RecordBarrierWaitStorageWriteBeforeReadWrite(uint32_t sessionID);

		private: // Methods
			// Delete all constructors:
			Async() = delete;
			Async(const Async&) = delete;
			Async& operator=(const Async&) = delete;
			Async(Async&&) = delete;
			Async& operator=(Async&&) = delete;
			~Async() = delete;
		};

		class EMBER_CORE_API PostRender
		{
		private: // Members:
			static emberBackendInterface::ICompute::IPostRender* s_pIPostRender;

		public: // Methods:
			// Constructor/Destructor:
			static void Init(emberBackendInterface::ICompute::IPostRender* pIPostRender);
			static void Clear();

			// Workload recording:
			// Post render compute is render-target sized by design; threadCount is derived by the backend.
			static ShaderProperties RecordComputeShader(ComputeShader& computeShader);
			static ShaderProperties RecordPostProcessingShader(ComputeShader& computeShader);

		private: // Methods
			// Delete all constructors:
			PostRender() = delete;
			PostRender(const PostRender&) = delete;
			PostRender& operator=(const PostRender&) = delete;
			PostRender(PostRender&&) = delete;
			PostRender& operator=(PostRender&&) = delete;
			~PostRender() = delete;
		};



		class EMBER_CORE_API PreRender
		{
		private: // Members:
			static emberBackendInterface::ICompute::IPreRender* s_pIPreRender;

		public: // Methods:
			// Constructor/Destructor:
			static void Init(emberBackendInterface::ICompute::IPreRender* pIPreRender);
			static void Clear();

			// Workload recording:
			static ShaderProperties RecordComputeShader(ComputeShader& computeShader, Uint3 threadCount);
			static void RecordBarrier(emberBackendInterface::ComputeBarrierFlag srcBarrierFlags, emberBackendInterface::ComputeBarrierFlag dstBarrierFlags);
			static void RecordBarrierWaitShaderWriteBeforeRead();
			static void RecordBarrierWaitStorageWriteBeforeRead();
			static void RecordBarrierWaitStorageWriteBeforeWrite();
			static void RecordBarrierWaitStorageWriteBeforeReadWrite();

		private: // Methods
			// Delete all constructors:
			PreRender() = delete;
			PreRender(const PreRender&) = delete;
			PreRender& operator=(const PreRender&) = delete;
			PreRender(PreRender&&) = delete;
			PreRender& operator=(PreRender&&) = delete;
			~PreRender() = delete;
		};



		class EMBER_CORE_API Physics
		{
		public: // Constants:
            // Async session id:    Ids used by async system. Get recycled via Compute::Async's poling system.
            // Physics session id:  Ids used to distinguish Compute::Physics sessions. Increments continuously each physics update.
            static constexpr uint32_t physicsUpdatesInFlight = 2;   // Don't change this. Tightly coupled to triple buffering.
			static constexpr uint64_t invalidPhysicsSessionID = static_cast<uint64_t>(-1);
			static constexpr uint32_t invalidAsyncSessionID = static_cast<uint32_t>(-1);

		private: // Members:
			static bool s_isRecording;
			static uint64_t s_recordingPhysicsSessionID;                // physics session id that is currently recording.
			static uint32_t s_recordingAsyncSessionID;                  // asyc backend id used for current recording.
			static std::array<uint64_t, physicsUpdatesInFlight> s_inFlightPhysicsSessionIDs; // keep track of physics session ids in flight.
			static std::array<uint32_t, physicsUpdatesInFlight> s_inFlightAsyncSessionIDs;   // keep track of async session ids in flight.
			static uint32_t s_sessionIndex;                             // shared index for the two arrays.

		public: // Methods:
			// Constructor/Destructor:
			static void Init();
			static void Clear();

			// Synchronization:
			static bool IsFinished(uint64_t physicsSessionID);
			static bool IsFinished();
			static void WaitForFinish(uint64_t physicsSessionID);
			static void WaitForFinish();

			// Workload recording:
			static void BeginRecording();
			static void EndRecording();
			static uint64_t GetRecordingSessionID();
			static ShaderProperties RecordComputeShader(ComputeShader& computeShader, Uint3 threadCount);
			static void RecordBarrier(emberBackendInterface::ComputeBarrierFlag srcBarrierFlags, emberBackendInterface::ComputeBarrierFlag dstBarrierFlags);
			static void RecordBarrierWaitShaderWriteBeforeRead();
			static void RecordBarrierWaitStorageWriteBeforeRead();
			static void RecordBarrierWaitStorageWriteBeforeWrite();
			static void RecordBarrierWaitStorageWriteBeforeReadWrite();

		private: // Methods
			// Delete all constructors:
			Physics() = delete;
			Physics(const Physics&) = delete;
			Physics& operator=(const Physics&) = delete;
			Physics(Physics&&) = delete;
			Physics& operator=(Physics&&) = delete;
			~Physics() = delete;
		};



	private: // Members:
		static bool s_isInitialized;
		static std::unique_ptr<emberBackendInterface::ICompute> s_pICompute;

	public: // Methods:
		// Initialization/Cleanup:
		static void Init(emberBackendInterface::ICompute* pICompute);
		static void Clear();

		// Workload recording (delegates to given computeType):
		static ShaderProperties RecordComputeShader(ComputeType computeType, ComputeShader& computeShader, Uint3 threadCount = Uint3::zero, uint32_t sessionID = -1);
		static void RecordBarrier(ComputeType computeType, emberBackendInterface::ComputeBarrierFlag srcBarrierFlags, emberBackendInterface::ComputeBarrierFlag dstBarrierFlags, uint32_t sessionID = -1);
		static void RecordBarrierWaitShaderWriteBeforeRead(ComputeType computeType, uint32_t sessionID = -1);
		static void RecordBarrierWaitStorageWriteBeforeRead(ComputeType computeType, uint32_t sessionID = -1);
		static void RecordBarrierWaitStorageWriteBeforeWrite(ComputeType computeType, uint32_t sessionID = -1);
		static void RecordBarrierWaitStorageWriteBeforeReadWrite(ComputeType computeType, uint32_t sessionID = -1);

	private: // Methods
		// Delete all constructors:
		Compute() = delete;
		Compute(const Compute&) = delete;
		Compute& operator=(const Compute&) = delete;
		Compute(Compute&&) = delete;
		Compute& operator=(Compute&&) = delete;
		~Compute() = delete;
	};
}