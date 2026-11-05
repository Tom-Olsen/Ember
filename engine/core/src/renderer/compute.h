#pragma once
#include "iCompute.h"
#include "commonPipelineStage.h"
#include "commonRendererCreateInfo.h"
#include "emberCoreExport.h"
#include "emberMath.h"
#include <array>
#include <vector>



namespace emberCore
{
	// Forward decleration:
	class ComputeShader;
	class Buffer;
	class Renderer;
	class CallProperties;



	// Allows for easier delegation to correct compute queue:
	enum class ComputeType
	{
		async,
		preRender,
		postOutline,
		postGeometry,
		postLighting,
		postRender,
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

			// Submission logic:
			static uint32_t CreateComputeSession();
			static void SubmitComputeSession(uint32_t sessionID);
			static void SubmitComputeSessionAndWait(uint32_t sessionID);
			static bool IsFinished(uint32_t sessionID);
			static void WaitForFinish(uint32_t sessionID);

			// Workload dispatch:
			static CallProperties DispatchComputeShader(uint32_t sessionID, ComputeShader& computeShader, Uint3 threadCount);
			static void DispatchBufferCopy(uint32_t sessionID, Buffer& source, Buffer& destination, uint64_t size, uint64_t sourceOffset = 0, uint64_t destinationOffset = 0);
			static void DispatchBufferBarrier(uint32_t sessionID, Buffer& buffer, emberCommon::PipelineStage sourceStages, emberCommon::ComputeBarrierFlag sourceAccess, emberCommon::PipelineStage destinationStages, emberCommon::ComputeBarrierFlag destinationAccess, uint64_t offset = 0, uint64_t size = UINT64_MAX);
			static void DispatchBarrier(uint32_t sessionID, emberCommon::ComputeBarrierFlag srcBarrierFlags, emberCommon::ComputeBarrierFlag dstBarrierFlags);
			static void DispatchBarrierWaitShaderWriteBeforeRead(uint32_t sessionID);
			static void DispatchBarrierWaitStorageWriteBeforeRead(uint32_t sessionID);
			static void DispatchBarrierWaitStorageWriteBeforeWrite(uint32_t sessionID);
			static void DispatchBarrierWaitStorageWriteBeforeReadWrite(uint32_t sessionID);
			static void DispatchBarrierWaitStorageWriteBeforeSampleReadStorageWrite(uint32_t sessionID);

		private: // Methods
			// Delete all constructors:
			Async() = delete;
			Async(const Async&) = delete;
			Async& operator=(const Async&) = delete;
			Async(Async&&) = delete;
			Async& operator=(Async&&) = delete;
			~Async() = delete;
		};



		class EMBER_CORE_API PreRender
		{
		private: // Members:
			static emberBackendInterface::ICompute::IQueue* s_pIPreRender;

		public: // Methods:
			// Constructor/Destructor:
			static void Init(emberBackendInterface::ICompute::IQueue* pIPreRender);
			static void Clear();

			// Workload dispatch:
			static CallProperties DispatchComputeShader(ComputeShader& computeShader, Uint3 threadCount);
			static void DispatchBufferCopy(Buffer& source, Buffer& destination, uint64_t size, uint64_t sourceOffset = 0, uint64_t destinationOffset = 0);
			static void DispatchBufferBarrier(Buffer& buffer, emberCommon::PipelineStage sourceStages, emberCommon::ComputeBarrierFlag sourceAccess, emberCommon::PipelineStage destinationStages, emberCommon::ComputeBarrierFlag destinationAccess, uint64_t offset = 0, uint64_t size = UINT64_MAX);
			static void DispatchBarrier(emberCommon::ComputeBarrierFlag srcBarrierFlags, emberCommon::ComputeBarrierFlag dstBarrierFlags);
			static void DispatchBarrierWaitShaderWriteBeforeRead();
			static void DispatchBarrierWaitStorageWriteBeforeRead();
			static void DispatchBarrierWaitStorageWriteBeforeWrite();
			static void DispatchBarrierWaitStorageWriteBeforeReadWrite();
			static void DispatchBarrierWaitStorageWriteBeforeSampleReadStorageWrite();

		private: // Methods
			// Delete all constructors:
			PreRender() = delete;
			PreRender(const PreRender&) = delete;
			PreRender& operator=(const PreRender&) = delete;
			PreRender(PreRender&&) = delete;
			PreRender& operator=(PreRender&&) = delete;
			~PreRender() = delete;
		};



		class EMBER_CORE_API PostOutline
		{
		private: // Members:
			static emberBackendInterface::ICompute::IQueue* s_pIPostOutline;

		public: // Methods:
			// Constructor/Destructor:
			static void Init(emberBackendInterface::ICompute::IQueue* pIPostOutline);
			static void Clear();

			// Workload dispatch:
			static CallProperties DispatchComputeShader(ComputeShader& computeShader, Uint3 threadCount);
			static void DispatchBufferCopy(Buffer& source, Buffer& destination, uint64_t size, uint64_t sourceOffset = 0, uint64_t destinationOffset = 0);
			static void DispatchBufferBarrier(Buffer& buffer, emberCommon::PipelineStage sourceStages, emberCommon::ComputeBarrierFlag sourceAccess, emberCommon::PipelineStage destinationStages, emberCommon::ComputeBarrierFlag destinationAccess, uint64_t offset = 0, uint64_t size = UINT64_MAX);
			static void DispatchBarrier(emberCommon::ComputeBarrierFlag srcBarrierFlags, emberCommon::ComputeBarrierFlag dstBarrierFlags);
			static void DispatchBarrierWaitShaderWriteBeforeRead();
			static void DispatchBarrierWaitStorageWriteBeforeRead();
			static void DispatchBarrierWaitStorageWriteBeforeWrite();
			static void DispatchBarrierWaitStorageWriteBeforeReadWrite();
			static void DispatchBarrierWaitStorageWriteBeforeSampleReadStorageWrite();

		private: // Methods
			// Delete all constructors:
			PostOutline() = delete;
			PostOutline(const PostOutline&) = delete;
			PostOutline& operator=(const PostOutline&) = delete;
			PostOutline(PostOutline&&) = delete;
			PostOutline& operator=(PostOutline&&) = delete;
			~PostOutline() = delete;
		};


		
		class EMBER_CORE_API PostGeometry
		{
		private: // Members:
			static emberBackendInterface::ICompute::IQueue* s_pIPostGeometry;

		public: // Methods:
			// Constructor/Destructor:
			static void Init(emberBackendInterface::ICompute::IQueue* pIPostGeometry);
			static void Clear();

			// Workload dispatch:
			static CallProperties DispatchComputeShader(ComputeShader& computeShader, Uint3 threadCount);
			static void DispatchBufferCopy(Buffer& source, Buffer& destination, uint64_t size, uint64_t sourceOffset = 0, uint64_t destinationOffset = 0);
			static void DispatchBufferBarrier(Buffer& buffer, emberCommon::PipelineStage sourceStages, emberCommon::ComputeBarrierFlag sourceAccess, emberCommon::PipelineStage destinationStages, emberCommon::ComputeBarrierFlag destinationAccess, uint64_t offset = 0, uint64_t size = UINT64_MAX);
			static void DispatchBarrier(emberCommon::ComputeBarrierFlag srcBarrierFlags, emberCommon::ComputeBarrierFlag dstBarrierFlags);
			static void DispatchBarrierWaitShaderWriteBeforeRead();
			static void DispatchBarrierWaitStorageWriteBeforeRead();
			static void DispatchBarrierWaitStorageWriteBeforeWrite();
			static void DispatchBarrierWaitStorageWriteBeforeReadWrite();
			static void DispatchBarrierWaitStorageWriteBeforeSampleReadStorageWrite();

		private: // Methods
			// Delete all constructors:
			PostGeometry() = delete;
			PostGeometry(const PostGeometry&) = delete;
			PostGeometry& operator=(const PostGeometry&) = delete;
			PostGeometry(PostGeometry&&) = delete;
			PostGeometry& operator=(PostGeometry&&) = delete;
			~PostGeometry() = delete;
		};



		class EMBER_CORE_API PostLighting
		{
		private: // Members:
			static emberBackendInterface::ICompute::IQueue* s_pIPostLighting;

		public: // Methods:
			// Constructor/Destructor:
			static void Init(emberBackendInterface::ICompute::IQueue* pIPostLighting);
			static void Clear();

			// Workload dispatch:
			static CallProperties DispatchComputeShader(ComputeShader& computeShader, Uint3 threadCount);
			static void DispatchBufferCopy(Buffer& source, Buffer& destination, uint64_t size, uint64_t sourceOffset = 0, uint64_t destinationOffset = 0);
			static void DispatchBufferBarrier(Buffer& buffer, emberCommon::PipelineStage sourceStages, emberCommon::ComputeBarrierFlag sourceAccess, emberCommon::PipelineStage destinationStages, emberCommon::ComputeBarrierFlag destinationAccess, uint64_t offset = 0, uint64_t size = UINT64_MAX);
			static void DispatchBarrier(emberCommon::ComputeBarrierFlag srcBarrierFlags, emberCommon::ComputeBarrierFlag dstBarrierFlags);
			static void DispatchBarrierWaitShaderWriteBeforeRead();
			static void DispatchBarrierWaitStorageWriteBeforeRead();
			static void DispatchBarrierWaitStorageWriteBeforeWrite();
			static void DispatchBarrierWaitStorageWriteBeforeReadWrite();
			static void DispatchBarrierWaitStorageWriteBeforeSampleReadStorageWrite();

		private: // Methods
			// Delete all constructors:
			PostLighting() = delete;
			PostLighting(const PostLighting&) = delete;
			PostLighting& operator=(const PostLighting&) = delete;
			PostLighting(PostLighting&&) = delete;
			PostLighting& operator=(PostLighting&&) = delete;
			~PostLighting() = delete;
		};



		class EMBER_CORE_API PostRender
		{
			friend class Renderer;

		private: // Members:
			static emberBackendInterface::ICompute::IQueue* s_pIPostRender;

		public: // Methods:
			// Constructor/Destructor:
			static void Init(emberBackendInterface::ICompute::IQueue* pIPostRender);
			static void Clear();

			// Workload dispatch:
			static CallProperties DispatchComputeShader(ComputeShader& computeShader, Uint3 threadCount);
			static CallProperties DispatchPostProcessingShader(ComputeShader& computeShader, Uint3 threadCount);
			static void DispatchBufferCopy(Buffer& source, Buffer& destination, uint64_t size, uint64_t sourceOffset = 0, uint64_t destinationOffset = 0);
			static void DispatchBufferBarrier(Buffer& buffer, emberCommon::PipelineStage sourceStages, emberCommon::ComputeBarrierFlag sourceAccess, emberCommon::PipelineStage destinationStages, emberCommon::ComputeBarrierFlag destinationAccess, uint64_t offset = 0, uint64_t size = UINT64_MAX);
			static void DispatchBarrier(emberCommon::ComputeBarrierFlag srcBarrierFlags, emberCommon::ComputeBarrierFlag dstBarrierFlags);
			static void DispatchBarrierWaitShaderWriteBeforeRead();
			static void DispatchBarrierWaitStorageWriteBeforeRead();
			static void DispatchBarrierWaitStorageWriteBeforeWrite();
			static void DispatchBarrierWaitStorageWriteBeforeReadWrite();
			static void DispatchBarrierWaitStorageWriteBeforeSampleReadStorageWrite();

		private: // Methods
			// Delete all constructors:
			PostRender() = delete;
			PostRender(const PostRender&) = delete;
			PostRender& operator=(const PostRender&) = delete;
			PostRender(PostRender&&) = delete;
			PostRender& operator=(PostRender&&) = delete;
			~PostRender() = delete;

			// Interface handle:
			static emberBackendInterface::ICompute::IQueue* GetInterfaceHandle();
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
			static bool s_isSessionActive;
			static uint64_t s_activePhysicsSessionID;                // physics session id currently accepting dispatched work.
			static uint32_t s_activeAsyncSessionID;                  // async backend id used for the active physics session.
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

			// Workload dispatch:
			static void BeginSession();
			static void EndSession();
			static uint64_t GetActiveSessionID();
			static CallProperties DispatchComputeShader(ComputeShader& computeShader, Uint3 threadCount);
			static void DispatchBufferCopy(Buffer& source, Buffer& destination, uint64_t size, uint64_t sourceOffset = 0, uint64_t destinationOffset = 0);
			static void DispatchBufferBarrier(Buffer& buffer, emberCommon::PipelineStage sourceStages, emberCommon::ComputeBarrierFlag sourceAccess, emberCommon::PipelineStage destinationStages, emberCommon::ComputeBarrierFlag destinationAccess, uint64_t offset = 0, uint64_t size = UINT64_MAX);
			static void DispatchBarrier(emberCommon::ComputeBarrierFlag srcBarrierFlags, emberCommon::ComputeBarrierFlag dstBarrierFlags);
			static void DispatchBarrierWaitShaderWriteBeforeRead();
			static void DispatchBarrierWaitStorageWriteBeforeRead();
			static void DispatchBarrierWaitStorageWriteBeforeWrite();
			static void DispatchBarrierWaitStorageWriteBeforeReadWrite();
			static void DispatchBarrierWaitStorageWriteBeforeSampleReadStorageWrite();

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
		static emberBackendInterface::ICompute* s_pICompute;

	public: // Methods:
		// Initialization/Cleanup:
		static void Init(emberBackendInterface::ICompute* pICompute);
		static void Clear();

		// Workload dispatch (delegates to given computeType):
		static CallProperties DispatchComputeShader(ComputeType computeType, ComputeShader& computeShader, Uint3 threadCount, uint32_t sessionID = -1);
		static void DispatchBufferCopy(ComputeType computeType, Buffer& source, Buffer& destination, uint64_t size, uint64_t sourceOffset = 0, uint64_t destinationOffset = 0, uint32_t sessionID = -1);
		static void DispatchBufferBarrier(ComputeType computeType, Buffer& buffer, emberCommon::PipelineStage sourceStages, emberCommon::ComputeBarrierFlag sourceAccess, emberCommon::PipelineStage destinationStages, emberCommon::ComputeBarrierFlag destinationAccess, uint64_t offset = 0, uint64_t size = UINT64_MAX, uint32_t sessionID = -1);
		static void DispatchBarrier(ComputeType computeType, emberCommon::ComputeBarrierFlag srcBarrierFlags, emberCommon::ComputeBarrierFlag dstBarrierFlags, uint32_t sessionID = -1);
		static void DispatchBarrierWaitShaderWriteBeforeRead(ComputeType computeType, uint32_t sessionID = -1);
		static void DispatchBarrierWaitStorageWriteBeforeRead(ComputeType computeType, uint32_t sessionID = -1);
		static void DispatchBarrierWaitStorageWriteBeforeWrite(ComputeType computeType, uint32_t sessionID = -1);
		static void DispatchBarrierWaitStorageWriteBeforeReadWrite(ComputeType computeType, uint32_t sessionID = -1);
        static void DispatchBarrierWaitStorageWriteBeforeSampleReadStorageWrite(ComputeType computeType, uint32_t sessionID = -1);

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