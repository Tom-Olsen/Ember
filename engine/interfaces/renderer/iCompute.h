#pragma once
#include "commonComputeBarrierFlag.h"
#include "commonPipelineStage.h"
#include "emberMath.h"
#include <cstdint>



namespace emberBackendInterface
{
	// Forward declarations:
	class IBuffer;
	class IComputeShader;
	class IDescriptorSetBinding;



	class ICompute
	{
	public: // Classes:
		class IQueue
		{
		public: // Methods:
			// Virtual destructor for v-table:
			virtual ~IQueue() = default;

			// Workload recording:
			virtual IDescriptorSetBinding* RecordComputeShader(IComputeShader* pComputeShader, Uint3 threadCount) = 0;
			virtual void RecordBufferCopy(IBuffer* pSource, IBuffer* pDestination, uint64_t size, uint64_t sourceOffset = 0, uint64_t destinationOffset = 0) = 0;
			virtual void RecordBufferBarrier(IBuffer* pBuffer, emberCommon::PipelineStage sourceStages, emberCommon::ComputeBarrierFlag sourceAccess, emberCommon::PipelineStage destinationStages, emberCommon::ComputeBarrierFlag destinationAccess, uint64_t offset = 0, uint64_t size = UINT64_MAX) = 0;
			virtual void RecordBarrier(emberCommon::ComputeBarrierFlag srcBarrierFlags, emberCommon::ComputeBarrierFlag dstBarrierFlags) = 0;
		};
		class IAsync
		{
		public: // Methods:
			// Virtual destructor for v-table:
			virtual ~IAsync() = default;

			// Dispatch logic:
			virtual uint32_t CreateComputeSession() = 0;
			virtual void DispatchComputeSession(uint32_t sessionID, float time, float deltaTime) = 0;
			virtual bool IsFinished(uint32_t sessionID) = 0;
			virtual void WaitForFinish(uint32_t sessionID) = 0;

			// Workload recording:
			virtual IQueue& GetRecordingQueue(uint32_t sessionID) = 0;
		};

	public: // Methods:
		// Virtual destructor for v-table:
		virtual ~ICompute() = default;

		// Getters:
		virtual IAsync* GetAsyncComputeInterfaceHandle() = 0;
		virtual IQueue* GetPreRenderComputeInterfaceHandle() = 0;
		virtual IQueue* GetPostOutlineComputeInterfaceHandle() = 0;
		virtual IQueue* GetPostGeometryComputeInterfaceHandle() = 0;
		virtual IQueue* GetPostLightingComputeInterfaceHandle() = 0;
		virtual IQueue* GetPostRenderComputeInterfaceHandle() = 0;
	};
}