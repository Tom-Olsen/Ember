#include "compute.h"
#include "computeShader.h"
#include "emberTime.h"
#include "logger.h"
#include "shaderProperties.h"
#include "iCompute.h"
#include "iComputeShader.h"
#include <stdexcept>



namespace emberEngine
{
	using ComputeBarrierFlag = emberBackendInterface::ComputeBarrierFlag;



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

	// Dispatch logic:
	uint32_t Compute::Async::CreateComputeSession()
	{
		return s_pIAsync->CreateComputeSession();
	}
	void Compute::Async::DispatchComputeSession(uint32_t sessionID)
	{
		s_pIAsync->DispatchComputeSession(sessionID, Time::GetTime(), Time::GetDeltaTime());
	}
	void Compute::Async::DispatchComputeSessionAndWait(uint32_t sessionID)
	{
		DispatchComputeSession(sessionID);
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

	// Workload recording:
	ShaderProperties Compute::Async::RecordComputeShader(uint32_t sessionID, ComputeShader& computeShader, Uint3 threadCount)
	{
		emberBackendInterface::IComputeShader* pIComputeShader = computeShader.GetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pIDescriptorSetBinding = s_pIAsync->RecordComputeShader(sessionID, pIComputeShader, threadCount);
		ShaderProperties shaderProperties = ShaderProperties(pIDescriptorSetBinding);
		return shaderProperties;
	}
	void Compute::Async::RecordBarrier(uint32_t sessionID, ComputeBarrierFlag srcBarrierFlags, ComputeBarrierFlag dstBarrierFlags)
	{
		s_pIAsync->RecordBarrier(sessionID, srcBarrierFlags, dstBarrierFlags);
	}
	void Compute::Async::RecordBarrierWaitShaderWriteBeforeRead(uint32_t sessionID)
	{
		Compute::Async::RecordBarrier(sessionID, ComputeBarrierFlag::shaderWrite, ComputeBarrierFlag::shaderRead);
	}
	void Compute::Async::RecordBarrierWaitStorageWriteBeforeRead(uint32_t sessionID)
	{
		Compute::Async::RecordBarrier(sessionID, ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageRead);
	}
	void Compute::Async::RecordBarrierWaitStorageWriteBeforeWrite(uint32_t sessionID)
	{
		Compute::Async::RecordBarrier(sessionID, ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageWrite);
	}
	void Compute::Async::RecordBarrierWaitStorageWriteBeforeReadWrite(uint32_t sessionID)
	{
		Compute::Async::RecordBarrier(sessionID, ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageRead | ComputeBarrierFlag::storageWrite);
	}

	// Compute::PostRender subclass:
	emberBackendInterface::ICompute::IPostRender* Compute::PostRender::s_pIPostRender;
	// Public:
	// Constructor/Destructor:
	void Compute::PostRender::Init(emberBackendInterface::ICompute::IPostRender* pIPostRender)
	{
		s_pIPostRender = pIPostRender;
	}
	void Compute::PostRender::Clear()
	{

	}

	// Workload recording:
	ShaderProperties Compute::PostRender::RecordComputeShader(ComputeShader& computeShader)
	{
		emberBackendInterface::IComputeShader* pIComputeShader = computeShader.GetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pIComputeCallDescriptorSetBinding = s_pIPostRender->RecordComputeShader(pIComputeShader);
		ShaderProperties shaderProperties = ShaderProperties(pIComputeCallDescriptorSetBinding);
		return shaderProperties;
	}



	// Compute::PreRender subclass:
	emberBackendInterface::ICompute::IPreRender* Compute::PreRender::s_pIPreRender;
	// Public:
	// Constructor/Destructor:
	void Compute::PreRender::Init(emberBackendInterface::ICompute::IPreRender* pIPreRender)
	{
		s_pIPreRender = pIPreRender;
	}
	void Compute::PreRender::Clear()
	{

	}

	// Workload recording:
	ShaderProperties Compute::PreRender::RecordComputeShader(ComputeShader& computeShader, Uint3 threadCount)
	{
		emberBackendInterface::IComputeShader* pIComputeShader = computeShader.GetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pIComputeCallDescriptorSetBinding = s_pIPreRender->RecordComputeShader(pIComputeShader, threadCount);
		ShaderProperties shaderProperties = ShaderProperties(pIComputeCallDescriptorSetBinding);
		return shaderProperties;
	}
	void Compute::PreRender::RecordBarrier(ComputeBarrierFlag srcBarrierFlags, ComputeBarrierFlag dstBarrierFlags)
	{
		s_pIPreRender->RecordBarrier(srcBarrierFlags, dstBarrierFlags);
	}
	void Compute::PreRender::RecordBarrierWaitShaderWriteBeforeRead()
	{
		Compute::PreRender::RecordBarrier(ComputeBarrierFlag::shaderWrite, ComputeBarrierFlag::shaderRead);
	}
	void Compute::PreRender::RecordBarrierWaitStorageWriteBeforeRead()
	{
		Compute::PreRender::RecordBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageRead);
	}
	void Compute::PreRender::RecordBarrierWaitStorageWriteBeforeWrite()
	{
		Compute::PreRender::RecordBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageWrite);
	}
	void Compute::PreRender::RecordBarrierWaitStorageWriteBeforeReadWrite()
	{
		Compute::PreRender::RecordBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageRead | ComputeBarrierFlag::storageWrite);
	}



	// Compute::Physics subclass:
	bool Compute::Physics::s_isRecording;
	uint64_t Compute::Physics::s_recordingPhysicsSessionID;
	uint32_t Compute::Physics::s_recordingAsyncSessionID;
	std::array<uint64_t, Compute::Physics::physicsUpdatesInFlight> Compute::Physics::s_inFlightPhysicsSessionIDs;
	std::array<uint32_t, Compute::Physics::physicsUpdatesInFlight> Compute::Physics::s_inFlightAsyncSessionIDs;
	uint32_t Compute::Physics::s_sessionIndex;
	// Public:
	// Constructor/Destructor:
	void Compute::Physics::Init()
	{
		s_isRecording = false;
		s_recordingPhysicsSessionID = invalidPhysicsSessionID;
		s_recordingAsyncSessionID = invalidAsyncSessionID;
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
        // physicsSessionID is currently recording:
		if (s_isRecording && physicsSessionID == s_recordingPhysicsSessionID)
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
        // physicsSessionID is currently recording:
		if (s_isRecording && physicsSessionID == s_recordingPhysicsSessionID)
		{
			LOG_WARN("Compute::Physics::WaitForFinish cannot wait for a session that is still recording.");
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
    
	// Workload recording:
	void Compute::Physics::BeginRecording()
	{
		if (s_isRecording)
		{
			LOG_WARN("Compute::Physics::BeginRecording called while already recording.");
			return;
		}
		if (s_inFlightPhysicsSessionIDs[s_sessionIndex] != invalidPhysicsSessionID)
			WaitForFinish(s_inFlightPhysicsSessionIDs[s_sessionIndex]);

		s_isRecording = true;
		s_recordingPhysicsSessionID++;
		s_recordingAsyncSessionID = Async::CreateComputeSession();
	}
	void Compute::Physics::EndRecording()
	{
		if (!s_isRecording)
		{
			LOG_WARN("Compute::Physics::EndRecording called while not recording.");
			return;
		}

		Async::DispatchComputeSession(s_recordingAsyncSessionID);
		s_inFlightPhysicsSessionIDs[s_sessionIndex] = s_recordingPhysicsSessionID;
		s_inFlightAsyncSessionIDs[s_sessionIndex] = s_recordingAsyncSessionID;
		s_sessionIndex = (s_sessionIndex + 1) % physicsUpdatesInFlight;
		s_isRecording = false;
		s_recordingAsyncSessionID = invalidAsyncSessionID;
	}
	uint64_t Compute::Physics::GetRecordingSessionID()
	{
		return s_isRecording ? s_recordingPhysicsSessionID : invalidPhysicsSessionID;
	}
	ShaderProperties Compute::Physics::RecordComputeShader(ComputeShader& computeShader, Uint3 threadCount)
	{
		if (!s_isRecording)
		{
			LOG_ERROR("Compute::Physics::RecordComputeShader called outside BeginRecording/EndRecording.");
			return ShaderProperties();
		}
		return Async::RecordComputeShader(s_recordingAsyncSessionID, computeShader, threadCount);
	}
	void Compute::Physics::RecordBarrier(emberBackendInterface::ComputeBarrierFlag srcBarrierFlags, emberBackendInterface::ComputeBarrierFlag dstBarrierFlags)
	{
		if (!s_isRecording)
		{
			LOG_ERROR("Compute::Physics::RecordBarrier called outside BeginRecording/EndRecording.");
			return;
		}
		Async::RecordBarrier(s_recordingAsyncSessionID, srcBarrierFlags, dstBarrierFlags);
	}
	void Compute::Physics::RecordBarrierWaitShaderWriteBeforeRead()
	{
		RecordBarrier(ComputeBarrierFlag::shaderWrite, ComputeBarrierFlag::shaderRead);
	}
	void Compute::Physics::RecordBarrierWaitStorageWriteBeforeRead()
	{
		RecordBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageRead);
	}
	void Compute::Physics::RecordBarrierWaitStorageWriteBeforeWrite()
	{
		RecordBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageWrite);
	}
	void Compute::Physics::RecordBarrierWaitStorageWriteBeforeReadWrite()
	{
		RecordBarrier(ComputeBarrierFlag::storageWrite, ComputeBarrierFlag::storageRead | ComputeBarrierFlag::storageWrite);
	}

	// Private:
	// Compute class:
	// Static members:
	bool Compute::s_isInitialized = false;
	std::unique_ptr<emberBackendInterface::ICompute> Compute::s_pICompute;



	// Public:
	// Initialization/Cleanup:
	void Compute::Init(emberBackendInterface::ICompute* pICompute)
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		s_pICompute = std::unique_ptr<emberBackendInterface::ICompute>(pICompute);
		Compute::Async::Init(s_pICompute->GetAsyncComputeInterfaceHandle());
		Compute::PostRender::Init(s_pICompute->GetPostRenderComputeInterfaceHandle());
		Compute::PreRender::Init(s_pICompute->GetPreRenderComputeInterfaceHandle());
		Compute::Physics::Init();
	}
	void Compute::Clear()
	{
		Compute::Physics::Clear();
		Compute::PreRender::Clear();
		Compute::PostRender::Clear();
		Compute::Async::Clear();
		s_pICompute.reset();
		s_isInitialized = false;
	}



	// Workload recording:
	ShaderProperties Compute::RecordComputeShader(ComputeType computeType, ComputeShader& computeShader, Uint3 threadCount, uint32_t sessionID)
	{
		switch (computeType)
		{
			case ComputeType::async:
				return Async::RecordComputeShader(sessionID, computeShader, threadCount);
				break;
			case ComputeType::postRender:
				// Post render compute is render-target sized by design, so threadCount is ignored here.
				return PostRender::RecordComputeShader(computeShader);
				break;
			case ComputeType::preRender:
				return PreRender::RecordComputeShader(computeShader, threadCount);
				break;
			case ComputeType::physics:
				return Physics::RecordComputeShader(computeShader, threadCount);
				break;
		}
		throw std::runtime_error("Compute::RecordComputeShader: invalid ComputeType.");
	}
	void Compute::RecordBarrier(ComputeType computeType, ComputeBarrierFlag srcBarrierFlags, ComputeBarrierFlag dstBarrierFlags, uint32_t sessionID)
	{
		switch (computeType)
		{
			case ComputeType::async:
				Async::RecordBarrier(sessionID, srcBarrierFlags, dstBarrierFlags);
				break;
			case ComputeType::postRender:
				break;
			case ComputeType::preRender:
				PreRender::RecordBarrier(srcBarrierFlags, dstBarrierFlags);
				break;
			case ComputeType::physics:
				Physics::RecordBarrier(srcBarrierFlags, dstBarrierFlags);
				break;
		}
	}
	void Compute::RecordBarrierWaitShaderWriteBeforeRead(ComputeType computeType, uint32_t sessionID)
	{
		switch (computeType)
		{
			case ComputeType::async:
				Async::RecordBarrierWaitShaderWriteBeforeRead(sessionID);
				break;
			case ComputeType::postRender:
				break;
			case ComputeType::preRender:
				PreRender::RecordBarrierWaitShaderWriteBeforeRead();
				break;
			case ComputeType::physics:
				Physics::RecordBarrierWaitShaderWriteBeforeRead();
				break;
		}
	}
	void Compute::RecordBarrierWaitStorageWriteBeforeRead(ComputeType computeType, uint32_t sessionID)
	{
		switch (computeType)
		{
			case ComputeType::async:
				Async::RecordBarrierWaitStorageWriteBeforeRead(sessionID);
				break;
			case ComputeType::postRender:
				break;
			case ComputeType::preRender:
				PreRender::RecordBarrierWaitStorageWriteBeforeRead();
				break;
			case ComputeType::physics:
				Physics::RecordBarrierWaitStorageWriteBeforeRead();
				break;
		}
	}
	void Compute::RecordBarrierWaitStorageWriteBeforeWrite(ComputeType computeType, uint32_t sessionID)
	{
		switch (computeType)
		{
			case ComputeType::async:
				Async::RecordBarrierWaitStorageWriteBeforeWrite(sessionID);
				break;
			case ComputeType::postRender:
				break;
			case ComputeType::preRender:
				PreRender::RecordBarrierWaitStorageWriteBeforeWrite();
				break;
			case ComputeType::physics:
				Physics::RecordBarrierWaitStorageWriteBeforeWrite();
				break;
		}
	}
	void Compute::RecordBarrierWaitStorageWriteBeforeReadWrite(ComputeType computeType, uint32_t sessionID)
	{
		switch (computeType)
		{
			case ComputeType::async:
				Async::RecordBarrierWaitStorageWriteBeforeReadWrite(sessionID);
				break;
			case ComputeType::postRender:
				break;
			case ComputeType::preRender:
				PreRender::RecordBarrierWaitStorageWriteBeforeReadWrite();
				break;
			case ComputeType::physics:
				Physics::RecordBarrierWaitStorageWriteBeforeReadWrite();
				break;
		}
	}
}