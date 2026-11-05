#include "compute.h"
#include "buffer.h"
#include "callProperties.h"
#include "computeShader.h"
#include "emberTime.h"
#include "iCompute.h"
#include "iComputeShader.h"
#include "logger.h"
#include <stdexcept>



namespace emberCore
{
	using ComputeBarrierFlag = emberCommon::ComputeBarrierFlag;



	// Compute::Async subclass:
	emberBackendInterface::ICompute::IAsync* Compute::Async::s_pIAsync;
	// Public:
	// Constructor/Destructor:
	void Compute::Async::Init(emberBackendInterface::ICompute::IAsync* pIAsync)
	{
		s_pIAsync = pIAsync;
	}
	void Compute::Async::Clear()
	{

	}

	// Submission logic:
	uint32_t Compute::Async::CreateComputeSession()
	{
		return s_pIAsync->CreateComputeSession();
	}
	void Compute::Async::SubmitComputeSession(uint32_t sessionID)
	{
		s_pIAsync->SubmitComputeSession(sessionID, Time::GetTime(), Time::GetDeltaTime());
	}
	void Compute::Async::SubmitComputeSessionAndWait(uint32_t sessionID)
	{
		SubmitComputeSession(sessionID);
		WaitForFinish(sessionID);
	}
	bool Compute::Async::IsFinished(uint32_t sessionID)
	{
		return s_pIAsync->IsFinished(sessionID);
	}
	void Compute::Async::WaitForFinish(uint32_t sessionID)
	{
		s_pIAsync->WaitForFinish(sessionID);
	}

	// Workload dispatch:
	CallProperties Compute::Async::DispatchComputeShader(uint32_t sessionID, ComputeShader& computeShader, Uint3 threadCount)
	{
		emberBackendInterface::IComputeShader* pIComputeShader = computeShader.TryGetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pIDescriptorSetBinding = s_pIAsync->GetActiveQueue(sessionID).DispatchComputeShader(pIComputeShader, threadCount);
		CallProperties callProperties = CallProperties(pIDescriptorSetBinding);
		return callProperties;
	}
	void Compute::Async::DispatchBufferCopy(uint32_t sessionID, Buffer& source, Buffer& destination, uint64_t size, uint64_t sourceOffset, uint64_t destinationOffset)
	{
		s_pIAsync->GetActiveQueue(sessionID).DispatchBufferCopy(source.GetInterfaceHandle(), destination.GetInterfaceHandle(), size, sourceOffset, destinationOffset);
	}
	void Compute::Async::DispatchBufferBarrier(uint32_t sessionID, Buffer& buffer, emberCommon::PipelineStage sourceStages, ComputeBarrierFlag sourceAccess, emberCommon::PipelineStage destinationStages, ComputeBarrierFlag destinationAccess, uint64_t offset, uint64_t size)
	{
		s_pIAsync->GetActiveQueue(sessionID).DispatchBufferBarrier(buffer.GetInterfaceHandle(), sourceStages, sourceAccess, destinationStages, destinationAccess, offset, size);
	}
	void Compute::Async::DispatchBarrier(uint32_t sessionID, ComputeBarrierFlag srcBarrierFlags, ComputeBarrierFlag dstBarrierFlags)
	{
		s_pIAsync->GetActiveQueue(sessionID).DispatchBarrier(srcBarrierFlags, dstBarrierFlags);
	}
	void Compute::Async::DispatchBarrierWaitShaderWriteBeforeRead(uint32_t sessionID)
	{
		DispatchBarrier(sessionID, ComputeBarrierFlag::shaderWrite, ComputeBarrierFlag::shaderRead);
	}
	void Compute::Async::DispatchBarrierWaitStorageWriteBeforeRead(uint32_t sessionID)
	{
		DispatchBarrier(sessionID, ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageRead);
	}
	void Compute::Async::DispatchBarrierWaitStorageWriteBeforeWrite(uint32_t sessionID)
	{
		DispatchBarrier(sessionID, ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageWrite);
	}
	void Compute::Async::DispatchBarrierWaitStorageWriteBeforeReadWrite(uint32_t sessionID)
	{
		DispatchBarrier(sessionID, ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageRead | ComputeBarrierFlag::storageWrite);
	}
	void Compute::Async::DispatchBarrierWaitStorageWriteBeforeSampleReadStorageWrite(uint32_t sessionID)
	{
		DispatchBarrier(sessionID, ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::sampledRead | ComputeBarrierFlag::storageWrite);
	}



	// Compute::PreRender subclass:
	emberBackendInterface::ICompute::IQueue* Compute::PreRender::s_pIPreRender;
	// Public:
	// Constructor/Destructor:
	void Compute::PreRender::Init(emberBackendInterface::ICompute::IQueue* pIPreRender)
	{
		s_pIPreRender = pIPreRender;
	}
	void Compute::PreRender::Clear()
	{

	}

	// Workload dispatch:
	CallProperties Compute::PreRender::DispatchComputeShader(ComputeShader& computeShader, Uint3 threadCount)
	{
		emberBackendInterface::IComputeShader* pIComputeShader = computeShader.TryGetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pIComputeCallDescriptorSetBinding = s_pIPreRender->DispatchComputeShader(pIComputeShader, threadCount);
		CallProperties callProperties = CallProperties(pIComputeCallDescriptorSetBinding);
		return callProperties;
	}
	void Compute::PreRender::DispatchBufferCopy(Buffer& source, Buffer& destination, uint64_t size, uint64_t sourceOffset, uint64_t destinationOffset)
	{
		s_pIPreRender->DispatchBufferCopy(source.GetInterfaceHandle(), destination.GetInterfaceHandle(), size, sourceOffset, destinationOffset);
	}
	void Compute::PreRender::DispatchBufferBarrier(Buffer& buffer, emberCommon::PipelineStage sourceStages, ComputeBarrierFlag sourceAccess, emberCommon::PipelineStage destinationStages, ComputeBarrierFlag destinationAccess, uint64_t offset, uint64_t size)
	{
		s_pIPreRender->DispatchBufferBarrier(buffer.GetInterfaceHandle(), sourceStages, sourceAccess, destinationStages, destinationAccess, offset, size);
	}
	void Compute::PreRender::DispatchBarrier(ComputeBarrierFlag srcBarrierFlags, ComputeBarrierFlag dstBarrierFlags)
	{
		s_pIPreRender->DispatchBarrier(srcBarrierFlags, dstBarrierFlags);
	}
	void Compute::PreRender::DispatchBarrierWaitShaderWriteBeforeRead()
	{
		DispatchBarrier(ComputeBarrierFlag::shaderWrite, ComputeBarrierFlag::shaderRead);
	}
	void Compute::PreRender::DispatchBarrierWaitStorageWriteBeforeRead()
	{
		DispatchBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageRead);
	}
	void Compute::PreRender::DispatchBarrierWaitStorageWriteBeforeWrite()
	{
		DispatchBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageWrite);
	}
	void Compute::PreRender::DispatchBarrierWaitStorageWriteBeforeReadWrite()
	{
		DispatchBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageRead | ComputeBarrierFlag::storageWrite);
	}
	void Compute::PreRender::DispatchBarrierWaitStorageWriteBeforeSampleReadStorageWrite()
	{
		DispatchBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::sampledRead | ComputeBarrierFlag::storageWrite);
	}



	// Compute::PostOutline subclass:
	emberBackendInterface::ICompute::IQueue* Compute::PostOutline::s_pIPostOutline;
	// Public:
	// Constructor/Destructor:
	void Compute::PostOutline::Init(emberBackendInterface::ICompute::IQueue* pIPostOutline)
	{
		s_pIPostOutline = pIPostOutline;
	}
	void Compute::PostOutline::Clear()
	{

	}

	// Workload dispatch:
	CallProperties Compute::PostOutline::DispatchComputeShader(ComputeShader& computeShader, Uint3 threadCount)
	{
		emberBackendInterface::IComputeShader* pIComputeShader = computeShader.TryGetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pIComputeCallDescriptorSetBinding = s_pIPostOutline->DispatchComputeShader(pIComputeShader, threadCount);
		CallProperties callProperties = CallProperties(pIComputeCallDescriptorSetBinding);
		return callProperties;
	}
	void Compute::PostOutline::DispatchBufferCopy(Buffer& source, Buffer& destination, uint64_t size, uint64_t sourceOffset, uint64_t destinationOffset)
	{
		s_pIPostOutline->DispatchBufferCopy(source.GetInterfaceHandle(), destination.GetInterfaceHandle(), size, sourceOffset, destinationOffset);
	}
	void Compute::PostOutline::DispatchBufferBarrier(Buffer& buffer, emberCommon::PipelineStage sourceStages, ComputeBarrierFlag sourceAccess, emberCommon::PipelineStage destinationStages, ComputeBarrierFlag destinationAccess, uint64_t offset, uint64_t size)
	{
		s_pIPostOutline->DispatchBufferBarrier(buffer.GetInterfaceHandle(), sourceStages, sourceAccess, destinationStages, destinationAccess, offset, size);
	}
	void Compute::PostOutline::DispatchBarrier(ComputeBarrierFlag srcBarrierFlags, ComputeBarrierFlag dstBarrierFlags)
	{
		s_pIPostOutline->DispatchBarrier(srcBarrierFlags, dstBarrierFlags);
	}
	void Compute::PostOutline::DispatchBarrierWaitShaderWriteBeforeRead()
	{
		DispatchBarrier(ComputeBarrierFlag::shaderWrite, ComputeBarrierFlag::shaderRead);
	}
	void Compute::PostOutline::DispatchBarrierWaitStorageWriteBeforeRead()
	{
		DispatchBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageRead);
	}
	void Compute::PostOutline::DispatchBarrierWaitStorageWriteBeforeWrite()
	{
		DispatchBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageWrite);
	}
	void Compute::PostOutline::DispatchBarrierWaitStorageWriteBeforeReadWrite()
	{
		DispatchBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageRead | ComputeBarrierFlag::storageWrite);
	}
	void Compute::PostOutline::DispatchBarrierWaitStorageWriteBeforeSampleReadStorageWrite()
	{
		DispatchBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::sampledRead | ComputeBarrierFlag::storageWrite);
	}


	
	// Compute::PostGeometry subclass:
	emberBackendInterface::ICompute::IQueue* Compute::PostGeometry::s_pIPostGeometry;
	// Public:
	// Constructor/Destructor:
	void Compute::PostGeometry::Init(emberBackendInterface::ICompute::IQueue* pIPostGeometry)
	{
		s_pIPostGeometry = pIPostGeometry;
	}
	void Compute::PostGeometry::Clear()
	{

	}

	// Workload dispatch:
	CallProperties Compute::PostGeometry::DispatchComputeShader(ComputeShader& computeShader, Uint3 threadCount)
	{
		emberBackendInterface::IComputeShader* pIComputeShader = computeShader.TryGetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pIComputeCallDescriptorSetBinding = s_pIPostGeometry->DispatchComputeShader(pIComputeShader, threadCount);
		CallProperties callProperties = CallProperties(pIComputeCallDescriptorSetBinding);
		return callProperties;
	}
	void Compute::PostGeometry::DispatchBufferCopy(Buffer& source, Buffer& destination, uint64_t size, uint64_t sourceOffset, uint64_t destinationOffset)
	{
		s_pIPostGeometry->DispatchBufferCopy(source.GetInterfaceHandle(), destination.GetInterfaceHandle(), size, sourceOffset, destinationOffset);
	}
	void Compute::PostGeometry::DispatchBufferBarrier(Buffer& buffer, emberCommon::PipelineStage sourceStages, ComputeBarrierFlag sourceAccess, emberCommon::PipelineStage destinationStages, ComputeBarrierFlag destinationAccess, uint64_t offset, uint64_t size)
	{
		s_pIPostGeometry->DispatchBufferBarrier(buffer.GetInterfaceHandle(), sourceStages, sourceAccess, destinationStages, destinationAccess, offset, size);
	}
	void Compute::PostGeometry::DispatchBarrier(ComputeBarrierFlag srcBarrierFlags, ComputeBarrierFlag dstBarrierFlags)
	{
		s_pIPostGeometry->DispatchBarrier(srcBarrierFlags, dstBarrierFlags);
	}
	void Compute::PostGeometry::DispatchBarrierWaitShaderWriteBeforeRead()
	{
		DispatchBarrier(ComputeBarrierFlag::shaderWrite, ComputeBarrierFlag::shaderRead);
	}
	void Compute::PostGeometry::DispatchBarrierWaitStorageWriteBeforeRead()
	{
		DispatchBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageRead);
	}
	void Compute::PostGeometry::DispatchBarrierWaitStorageWriteBeforeWrite()
	{
		DispatchBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageWrite);
	}
	void Compute::PostGeometry::DispatchBarrierWaitStorageWriteBeforeReadWrite()
	{
		DispatchBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageRead | ComputeBarrierFlag::storageWrite);
	}
	void Compute::PostGeometry::DispatchBarrierWaitStorageWriteBeforeSampleReadStorageWrite()
	{
		DispatchBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::sampledRead | ComputeBarrierFlag::storageWrite);
	}



	// Compute::PostLighting subclass:
	emberBackendInterface::ICompute::IQueue* Compute::PostLighting::s_pIPostLighting;
	// Public:
	// Constructor/Destructor:
	void Compute::PostLighting::Init(emberBackendInterface::ICompute::IQueue* pIPostLighting)
	{
		s_pIPostLighting = pIPostLighting;
	}
	void Compute::PostLighting::Clear()
	{

	}

	// Workload dispatch:
	CallProperties Compute::PostLighting::DispatchComputeShader(ComputeShader& computeShader, Uint3 threadCount)
	{
		emberBackendInterface::IComputeShader* pIComputeShader = computeShader.TryGetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pIComputeCallDescriptorSetBinding = s_pIPostLighting->DispatchComputeShader(pIComputeShader, threadCount);
		CallProperties callProperties = CallProperties(pIComputeCallDescriptorSetBinding);
		return callProperties;
	}
	void Compute::PostLighting::DispatchBufferCopy(Buffer& source, Buffer& destination, uint64_t size, uint64_t sourceOffset, uint64_t destinationOffset)
	{
		s_pIPostLighting->DispatchBufferCopy(source.GetInterfaceHandle(), destination.GetInterfaceHandle(), size, sourceOffset, destinationOffset);
	}
	void Compute::PostLighting::DispatchBufferBarrier(Buffer& buffer, emberCommon::PipelineStage sourceStages, ComputeBarrierFlag sourceAccess, emberCommon::PipelineStage destinationStages, ComputeBarrierFlag destinationAccess, uint64_t offset, uint64_t size)
	{
		s_pIPostLighting->DispatchBufferBarrier(buffer.GetInterfaceHandle(), sourceStages, sourceAccess, destinationStages, destinationAccess, offset, size);
	}
	void Compute::PostLighting::DispatchBarrier(ComputeBarrierFlag srcBarrierFlags, ComputeBarrierFlag dstBarrierFlags)
	{
		s_pIPostLighting->DispatchBarrier(srcBarrierFlags, dstBarrierFlags);
	}
	void Compute::PostLighting::DispatchBarrierWaitShaderWriteBeforeRead()
	{
		DispatchBarrier(ComputeBarrierFlag::shaderWrite, ComputeBarrierFlag::shaderRead);
	}
	void Compute::PostLighting::DispatchBarrierWaitStorageWriteBeforeRead()
	{
		DispatchBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageRead);
	}
	void Compute::PostLighting::DispatchBarrierWaitStorageWriteBeforeWrite()
	{
		DispatchBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageWrite);
	}
	void Compute::PostLighting::DispatchBarrierWaitStorageWriteBeforeReadWrite()
	{
		DispatchBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageRead | ComputeBarrierFlag::storageWrite);
	}
	void Compute::PostLighting::DispatchBarrierWaitStorageWriteBeforeSampleReadStorageWrite()
	{
		DispatchBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::sampledRead | ComputeBarrierFlag::storageWrite);
	}



	// Compute::PostRender subclass:
	emberBackendInterface::ICompute::IQueue* Compute::PostRender::s_pIPostRender;
	// Public:
	// Constructor/Destructor:
	void Compute::PostRender::Init(emberBackendInterface::ICompute::IQueue* pIPostRender)
	{
		s_pIPostRender = pIPostRender;
	}
	void Compute::PostRender::Clear()
	{
		s_pIPostRender = nullptr;
	}

	// Workload dispatch:
	CallProperties Compute::PostRender::DispatchComputeShader(ComputeShader& computeShader, Uint3 threadCount)
	{
		emberBackendInterface::IComputeShader* pIComputeShader = computeShader.TryGetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pIComputeCallDescriptorSetBinding = s_pIPostRender->DispatchComputeShader(pIComputeShader, threadCount);
		CallProperties callProperties = CallProperties(pIComputeCallDescriptorSetBinding);
		return callProperties;
	}
	CallProperties Compute::PostRender::DispatchPostProcessingShader(ComputeShader& computeShader, Uint3 threadCount)
	{
		emberBackendInterface::IComputeShader* pIComputeShader = computeShader.TryGetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pIComputeCallDescriptorSetBinding = s_pIPostRender->DispatchComputeShader(pIComputeShader, threadCount);
		CallProperties callProperties = CallProperties(pIComputeCallDescriptorSetBinding);
		return callProperties;
	}
	void Compute::PostRender::DispatchBufferCopy(Buffer& source, Buffer& destination, uint64_t size, uint64_t sourceOffset, uint64_t destinationOffset)
	{
		GetInterfaceHandle()->DispatchBufferCopy(source.GetInterfaceHandle(), destination.GetInterfaceHandle(), size, sourceOffset, destinationOffset);
	}
	void Compute::PostRender::DispatchBufferBarrier(Buffer& buffer, emberCommon::PipelineStage sourceStages, ComputeBarrierFlag sourceAccess, emberCommon::PipelineStage destinationStages, ComputeBarrierFlag destinationAccess, uint64_t offset, uint64_t size)
	{
		GetInterfaceHandle()->DispatchBufferBarrier(buffer.GetInterfaceHandle(), sourceStages, sourceAccess, destinationStages, destinationAccess, offset, size);
	}
	void Compute::PostRender::DispatchBarrier(ComputeBarrierFlag srcBarrierFlags, ComputeBarrierFlag dstBarrierFlags)
	{
		s_pIPostRender->DispatchBarrier(srcBarrierFlags, dstBarrierFlags);
	}
	void Compute::PostRender::DispatchBarrierWaitShaderWriteBeforeRead()
	{
		DispatchBarrier(ComputeBarrierFlag::shaderWrite, ComputeBarrierFlag::shaderRead);
	}
	void Compute::PostRender::DispatchBarrierWaitStorageWriteBeforeRead()
	{
		DispatchBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageRead);
	}
	void Compute::PostRender::DispatchBarrierWaitStorageWriteBeforeWrite()
	{
		DispatchBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageWrite);
	}
	void Compute::PostRender::DispatchBarrierWaitStorageWriteBeforeReadWrite()
	{
		DispatchBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageRead | ComputeBarrierFlag::storageWrite);
	}
	void Compute::PostRender::DispatchBarrierWaitStorageWriteBeforeSampleReadStorageWrite()
	{
		DispatchBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::sampledRead | ComputeBarrierFlag::storageWrite);
	}



	// Compute::PostRender private methods:
	// Interface handle:
	emberBackendInterface::ICompute::IQueue* Compute::PostRender::GetInterfaceHandle()
	{
		if (!s_pIPostRender)
			throw std::runtime_error("Compute::PostRender is not initialized.");
		return s_pIPostRender;
	}



	// Compute::Physics subclass:
	bool Compute::Physics::s_isSessionActive;
	uint64_t Compute::Physics::s_activePhysicsSessionID;
	uint32_t Compute::Physics::s_activeAsyncSessionID;
	std::array<uint64_t, Compute::Physics::physicsUpdatesInFlight> Compute::Physics::s_inFlightPhysicsSessionIDs;
	std::array<uint32_t, Compute::Physics::physicsUpdatesInFlight> Compute::Physics::s_inFlightAsyncSessionIDs;
	uint32_t Compute::Physics::s_sessionIndex;
	// Public:
	// Constructor/Destructor:
	void Compute::Physics::Init()
	{
		s_isSessionActive = false;
		s_activePhysicsSessionID = invalidPhysicsSessionID;
		s_activeAsyncSessionID = invalidAsyncSessionID;
		s_inFlightPhysicsSessionIDs.fill(invalidPhysicsSessionID);
		s_inFlightAsyncSessionIDs.fill(invalidAsyncSessionID);
		s_sessionIndex = 0;
	}
	void Compute::Physics::Clear()
	{
		WaitForFinish();
	}

	// Synchronization:
	bool Compute::Physics::IsFinished(uint64_t physicsSessionID)
	{
        // Invalid id always finished:
		if (physicsSessionID == invalidPhysicsSessionID)
			return true;
        // physicsSessionID has not been submitted yet:
		if (s_isSessionActive && physicsSessionID == s_activePhysicsSessionID)
			return false;

        // Check if physicsSessionID is among in flight physics sessions:
		for (uint32_t i = 0; i < s_inFlightPhysicsSessionIDs.size(); i++)
		{
			if (s_inFlightPhysicsSessionIDs[i] != physicsSessionID)
				continue;
            // Async::IsFinished(...) resets the compute session if finished. So we must invalidate in flight session ids here aswell.
			bool isFinished = Async::IsFinished(s_inFlightAsyncSessionIDs[i]);
			if (isFinished)
			{
				s_inFlightPhysicsSessionIDs[i] = invalidPhysicsSessionID;
				s_inFlightAsyncSessionIDs[i] = invalidAsyncSessionID;
			}
			return isFinished;
		}
		return true;
	}
	bool Compute::Physics::IsFinished()
	{
		for (uint32_t i = 0; i < s_inFlightPhysicsSessionIDs.size(); i++)
			if (!IsFinished(s_inFlightPhysicsSessionIDs[i]))
				return false;
		return true;
	}
	void Compute::Physics::WaitForFinish(uint64_t physicsSessionID)
	{
        // Invalid id always finished:
		if (physicsSessionID == invalidPhysicsSessionID)
			return;
        // physicsSessionID has not been submitted yet:
		if (s_isSessionActive && physicsSessionID == s_activePhysicsSessionID)
		{
			LOG_WARN("Compute::Physics::WaitForFinish cannot wait for a session that has not been submitted yet.");
			return;
		}

		for (uint32_t i = 0; i < s_inFlightPhysicsSessionIDs.size(); i++)
		{
			if (s_inFlightPhysicsSessionIDs[i] != physicsSessionID)
				continue;
			if (!Async::IsFinished(s_inFlightAsyncSessionIDs[i]))
			{
				LOG_WARN("Waiting for Compute::Physics session.");
				Async::WaitForFinish(s_inFlightAsyncSessionIDs[i]);
			}
			s_inFlightPhysicsSessionIDs[i] = invalidPhysicsSessionID;
			s_inFlightAsyncSessionIDs[i] = invalidAsyncSessionID;
			return;
		}
	}
	void Compute::Physics::WaitForFinish()
	{
		for (uint32_t i = 0; i < s_inFlightPhysicsSessionIDs.size(); i++)
			WaitForFinish(s_inFlightPhysicsSessionIDs[i]);
	}
    
	// Workload dispatch:
	void Compute::Physics::BeginSession()
	{
		if (s_isSessionActive)
		{
			LOG_WARN("Compute::Physics::BeginSession called while a session is already active.");
			return;
		}
		if (s_inFlightPhysicsSessionIDs[s_sessionIndex] != invalidPhysicsSessionID)
			WaitForFinish(s_inFlightPhysicsSessionIDs[s_sessionIndex]);

		s_isSessionActive = true;
		s_activePhysicsSessionID++;
		s_activeAsyncSessionID = Async::CreateComputeSession();
	}
	void Compute::Physics::EndSession()
	{
		if (!s_isSessionActive)
		{
			LOG_WARN("Compute::Physics::EndSession called without an active session.");
			return;
		}

		Async::SubmitComputeSession(s_activeAsyncSessionID);
		s_inFlightPhysicsSessionIDs[s_sessionIndex] = s_activePhysicsSessionID;
		s_inFlightAsyncSessionIDs[s_sessionIndex] = s_activeAsyncSessionID;
		s_sessionIndex = (s_sessionIndex + 1) % physicsUpdatesInFlight;
		s_isSessionActive = false;
		s_activeAsyncSessionID = invalidAsyncSessionID;
	}
	uint64_t Compute::Physics::GetActiveSessionID()
	{
		return s_isSessionActive ? s_activePhysicsSessionID : invalidPhysicsSessionID;
	}
	CallProperties Compute::Physics::DispatchComputeShader(ComputeShader& computeShader, Uint3 threadCount)
	{
		if (!s_isSessionActive)
		{
			LOG_ERROR("Compute::Physics::DispatchComputeShader called outside BeginSession/EndSession.");
			return CallProperties();
		}
		return Async::DispatchComputeShader(s_activeAsyncSessionID, computeShader, threadCount);
	}
	void Compute::Physics::DispatchBufferCopy(Buffer& source, Buffer& destination, uint64_t size, uint64_t sourceOffset, uint64_t destinationOffset)
	{
		if (!s_isSessionActive)
		{
			LOG_ERROR("Compute::Physics::DispatchBufferCopy called outside BeginSession/EndSession.");
			return;
		}
		Async::DispatchBufferCopy(s_activeAsyncSessionID, source, destination, size, sourceOffset, destinationOffset);
	}
	void Compute::Physics::DispatchBufferBarrier(Buffer& buffer, emberCommon::PipelineStage sourceStages, ComputeBarrierFlag sourceAccess, emberCommon::PipelineStage destinationStages, ComputeBarrierFlag destinationAccess, uint64_t offset, uint64_t size)
	{
		if (!s_isSessionActive)
		{
			LOG_ERROR("Compute::Physics::DispatchBufferBarrier called outside BeginSession/EndSession.");
			return;
		}
		Async::DispatchBufferBarrier(s_activeAsyncSessionID, buffer, sourceStages, sourceAccess, destinationStages, destinationAccess, offset, size);
	}
	void Compute::Physics::DispatchBarrier(emberCommon::ComputeBarrierFlag srcBarrierFlags, emberCommon::ComputeBarrierFlag dstBarrierFlags)
	{
		if (!s_isSessionActive)
		{
			LOG_ERROR("Compute::Physics::DispatchBarrier called outside BeginSession/EndSession.");
			return;
		}
		Async::DispatchBarrier(s_activeAsyncSessionID, srcBarrierFlags, dstBarrierFlags);
	}
	void Compute::Physics::DispatchBarrierWaitShaderWriteBeforeRead()
	{
		DispatchBarrier(ComputeBarrierFlag::shaderWrite, ComputeBarrierFlag::shaderRead);
	}
	void Compute::Physics::DispatchBarrierWaitStorageWriteBeforeRead()
	{
		DispatchBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageRead);
	}
	void Compute::Physics::DispatchBarrierWaitStorageWriteBeforeWrite()
	{
		DispatchBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageWrite);
	}
	void Compute::Physics::DispatchBarrierWaitStorageWriteBeforeReadWrite()
	{
		DispatchBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageRead | ComputeBarrierFlag::storageWrite);
	}
	void Compute::Physics::DispatchBarrierWaitStorageWriteBeforeSampleReadStorageWrite()
	{
		DispatchBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::sampledRead | ComputeBarrierFlag::storageWrite);
	}

	// Private:
	// Compute class:
	// Static members:
	bool Compute::s_isInitialized = false;
	emberBackendInterface::ICompute* Compute::s_pICompute = nullptr;



	// Public:
	// Initialization/Cleanup:
	void Compute::Init(emberBackendInterface::ICompute* pICompute)
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		s_pICompute = pICompute;
		Compute::Async::Init(s_pICompute->GetAsyncComputeInterfaceHandle());
		Compute::PreRender::Init(s_pICompute->GetPreRenderComputeInterfaceHandle());
		Compute::PostOutline::Init(s_pICompute->GetPostOutlineComputeInterfaceHandle());
		Compute::PostGeometry::Init(s_pICompute->GetPostGeometryComputeInterfaceHandle());
		Compute::PostLighting::Init(s_pICompute->GetPostLightingComputeInterfaceHandle());
		Compute::PostRender::Init(s_pICompute->GetPostRenderComputeInterfaceHandle());
		Compute::Physics::Init();
	}
	void Compute::Clear()
	{
		Compute::Physics::Clear();
		Compute::PostRender::Clear();
		Compute::PostLighting::Clear();
		Compute::PostGeometry::Clear();
		Compute::PostOutline::Clear();
		Compute::PreRender::Clear();
		Compute::Async::Clear();
		s_pICompute = nullptr;
		s_isInitialized = false;
	}



	// Workload dispatch:
	CallProperties Compute::DispatchComputeShader(ComputeType computeType, ComputeShader& computeShader, Uint3 threadCount, uint32_t sessionID)
	{
		switch (computeType)
		{
			case ComputeType::async:
				return Async::DispatchComputeShader(sessionID, computeShader, threadCount);
				break;
			case ComputeType::preRender:
				return PreRender::DispatchComputeShader(computeShader, threadCount);
				break;
			case ComputeType::postOutline:
				return PostOutline::DispatchComputeShader(computeShader, threadCount);
				break;
			case ComputeType::postGeometry:
				return PostGeometry::DispatchComputeShader(computeShader, threadCount);
				break;
			case ComputeType::postLighting:
				return PostLighting::DispatchComputeShader(computeShader, threadCount);
				break;
			case ComputeType::postRender:
				return PostRender::DispatchComputeShader(computeShader, threadCount);
				break;
			case ComputeType::physics:
				return Physics::DispatchComputeShader(computeShader, threadCount);
				break;
		}
		throw std::runtime_error("Compute::DispatchComputeShader: invalid ComputeType.");
	}
	void Compute::DispatchBufferCopy(ComputeType computeType, Buffer& source, Buffer& destination, uint64_t size, uint64_t sourceOffset, uint64_t destinationOffset, uint32_t sessionID)
	{
		switch (computeType)
		{
			case ComputeType::async:
				Async::DispatchBufferCopy(sessionID, source, destination, size, sourceOffset, destinationOffset);
				return;
			case ComputeType::preRender:
				PreRender::DispatchBufferCopy(source, destination, size, sourceOffset, destinationOffset);
				return;
			case ComputeType::postOutline:
				PostOutline::DispatchBufferCopy(source, destination, size, sourceOffset, destinationOffset);
				return;
			case ComputeType::postGeometry:
				PostGeometry::DispatchBufferCopy(source, destination, size, sourceOffset, destinationOffset);
				return;
			case ComputeType::postLighting:
				PostLighting::DispatchBufferCopy(source, destination, size, sourceOffset, destinationOffset);
				return;
			case ComputeType::postRender:
				PostRender::DispatchBufferCopy(source, destination, size, sourceOffset, destinationOffset);
				return;
			case ComputeType::physics:
				Physics::DispatchBufferCopy(source, destination, size, sourceOffset, destinationOffset);
				return;
		}
		throw std::runtime_error("Compute::DispatchBufferCopy: invalid ComputeType.");
	}
	void Compute::DispatchBufferBarrier(ComputeType computeType, Buffer& buffer, emberCommon::PipelineStage sourceStages, ComputeBarrierFlag sourceAccess, emberCommon::PipelineStage destinationStages, ComputeBarrierFlag destinationAccess, uint64_t offset, uint64_t size, uint32_t sessionID)
	{
		switch (computeType)
		{
			case ComputeType::async:
				Async::DispatchBufferBarrier(sessionID, buffer, sourceStages, sourceAccess, destinationStages, destinationAccess, offset, size);
				return;
			case ComputeType::preRender:
				PreRender::DispatchBufferBarrier(buffer, sourceStages, sourceAccess, destinationStages, destinationAccess, offset, size);
				return;
			case ComputeType::postOutline:
				PostOutline::DispatchBufferBarrier(buffer, sourceStages, sourceAccess, destinationStages, destinationAccess, offset, size);
				return;
			case ComputeType::postGeometry:
				PostGeometry::DispatchBufferBarrier(buffer, sourceStages, sourceAccess, destinationStages, destinationAccess, offset, size);
				return;
			case ComputeType::postLighting:
				PostLighting::DispatchBufferBarrier(buffer, sourceStages, sourceAccess, destinationStages, destinationAccess, offset, size);
				return;
			case ComputeType::postRender:
				PostRender::DispatchBufferBarrier(buffer, sourceStages, sourceAccess, destinationStages, destinationAccess, offset, size);
				return;
			case ComputeType::physics:
				Physics::DispatchBufferBarrier(buffer, sourceStages, sourceAccess, destinationStages, destinationAccess, offset, size);
				return;
		}
		throw std::runtime_error("Compute::DispatchBufferBarrier: invalid ComputeType.");
	}
	void Compute::DispatchBarrier(ComputeType computeType, ComputeBarrierFlag srcBarrierFlags, ComputeBarrierFlag dstBarrierFlags, uint32_t sessionID)
	{
		switch (computeType)
		{
			case ComputeType::async:
				Async::DispatchBarrier(sessionID, srcBarrierFlags, dstBarrierFlags);
				break;
			case ComputeType::preRender:
				PreRender::DispatchBarrier(srcBarrierFlags, dstBarrierFlags);
				break;
			case ComputeType::postOutline:
				PostOutline::DispatchBarrier(srcBarrierFlags, dstBarrierFlags);
				break;
			case ComputeType::postGeometry:
				PostGeometry::DispatchBarrier(srcBarrierFlags, dstBarrierFlags);
				break;
			case ComputeType::postLighting:
				PostLighting::DispatchBarrier(srcBarrierFlags, dstBarrierFlags);
				break;
			case ComputeType::postRender:
				PostRender::DispatchBarrier(srcBarrierFlags, dstBarrierFlags);
				break;
			case ComputeType::physics:
				Physics::DispatchBarrier(srcBarrierFlags, dstBarrierFlags);
				break;
		}
	}
	void Compute::DispatchBarrierWaitShaderWriteBeforeRead(ComputeType computeType, uint32_t sessionID)
	{
		switch (computeType)
		{
			case ComputeType::async:
				Async::DispatchBarrierWaitShaderWriteBeforeRead(sessionID);
				break;
			case ComputeType::preRender:
				PreRender::DispatchBarrierWaitShaderWriteBeforeRead();
				break;
			case ComputeType::postOutline:
				PostOutline::DispatchBarrierWaitShaderWriteBeforeRead();
				break;
			case ComputeType::postGeometry:
				PostGeometry::DispatchBarrierWaitShaderWriteBeforeRead();
				break;
			case ComputeType::postLighting:
				PostLighting::DispatchBarrierWaitShaderWriteBeforeRead();
				break;
			case ComputeType::postRender:
				PostRender::DispatchBarrierWaitShaderWriteBeforeRead();
				break;
			case ComputeType::physics:
				Physics::DispatchBarrierWaitShaderWriteBeforeRead();
				break;
		}
	}
	void Compute::DispatchBarrierWaitStorageWriteBeforeRead(ComputeType computeType, uint32_t sessionID)
	{
		switch (computeType)
		{
			case ComputeType::async:
				Async::DispatchBarrierWaitStorageWriteBeforeRead(sessionID);
				break;
			case ComputeType::preRender:
				PreRender::DispatchBarrierWaitStorageWriteBeforeRead();
				break;
			case ComputeType::postOutline:
				PostOutline::DispatchBarrierWaitStorageWriteBeforeRead();
				break;
			case ComputeType::postGeometry:
				PostGeometry::DispatchBarrierWaitStorageWriteBeforeRead();
				break;
			case ComputeType::postLighting:
				PostLighting::DispatchBarrierWaitStorageWriteBeforeRead();
				break;
			case ComputeType::postRender:
				PostRender::DispatchBarrierWaitStorageWriteBeforeRead();
				break;
			case ComputeType::physics:
				Physics::DispatchBarrierWaitStorageWriteBeforeRead();
				break;
		}
	}
	void Compute::DispatchBarrierWaitStorageWriteBeforeWrite(ComputeType computeType, uint32_t sessionID)
	{
		switch (computeType)
		{
			case ComputeType::async:
				Async::DispatchBarrierWaitStorageWriteBeforeWrite(sessionID);
				break;
			case ComputeType::preRender:
				PreRender::DispatchBarrierWaitStorageWriteBeforeWrite();
				break;
			case ComputeType::postOutline:
				PostOutline::DispatchBarrierWaitStorageWriteBeforeWrite();
				break;
			case ComputeType::postGeometry:
				PostGeometry::DispatchBarrierWaitStorageWriteBeforeWrite();
				break;
			case ComputeType::postLighting:
				PostLighting::DispatchBarrierWaitStorageWriteBeforeWrite();
				break;
			case ComputeType::postRender:
				PostRender::DispatchBarrierWaitStorageWriteBeforeWrite();
				break;
			case ComputeType::physics:
				Physics::DispatchBarrierWaitStorageWriteBeforeWrite();
				break;
		}
	}
	void Compute::DispatchBarrierWaitStorageWriteBeforeReadWrite(ComputeType computeType, uint32_t sessionID)
	{
		switch (computeType)
		{
			case ComputeType::async:
				Async::DispatchBarrierWaitStorageWriteBeforeReadWrite(sessionID);
				break;
			case ComputeType::preRender:
				PreRender::DispatchBarrierWaitStorageWriteBeforeReadWrite();
				break;
			case ComputeType::postOutline:
				PostOutline::DispatchBarrierWaitStorageWriteBeforeReadWrite();
				break;
			case ComputeType::postGeometry:
				PostGeometry::DispatchBarrierWaitStorageWriteBeforeReadWrite();
				break;
			case ComputeType::postLighting:
				PostLighting::DispatchBarrierWaitStorageWriteBeforeReadWrite();
				break;
			case ComputeType::postRender:
				PostRender::DispatchBarrierWaitStorageWriteBeforeReadWrite();
				break;
			case ComputeType::physics:
				Physics::DispatchBarrierWaitStorageWriteBeforeReadWrite();
				break;
		}
	}
	void Compute::DispatchBarrierWaitStorageWriteBeforeSampleReadStorageWrite(ComputeType computeType, uint32_t sessionID)
	{
		switch (computeType)
		{
			case ComputeType::async:
				Async::DispatchBarrierWaitStorageWriteBeforeSampleReadStorageWrite(sessionID);
				break;
			case ComputeType::preRender:
				PreRender::DispatchBarrierWaitStorageWriteBeforeSampleReadStorageWrite();
				break;
			case ComputeType::postOutline:
				PostOutline::DispatchBarrierWaitStorageWriteBeforeSampleReadStorageWrite();
				break;
			case ComputeType::postGeometry:
				PostGeometry::DispatchBarrierWaitStorageWriteBeforeSampleReadStorageWrite();
				break;
			case ComputeType::postLighting:
				PostLighting::DispatchBarrierWaitStorageWriteBeforeSampleReadStorageWrite();
				break;
			case ComputeType::postRender:
				PostRender::DispatchBarrierWaitStorageWriteBeforeSampleReadStorageWrite();
				break;
			case ComputeType::physics:
				Physics::DispatchBarrierWaitStorageWriteBeforeSampleReadStorageWrite();
				break;
		}
	}
}