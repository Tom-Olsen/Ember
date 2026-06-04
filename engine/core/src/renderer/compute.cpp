#include "compute.h"
#include "computeShader.h"
#include "emberTime.h"
#include "shaderProperties.h"
#include "iCompute.h"
#include "iComputeShader.h"
#include <stdexcept>



namespace emberEngine
{
	using ComputeBarrierFlag = emberBackendInterface::ComputeBarrierFlag;



	// Compute::Async subclass:
	emberBackendInterface::ICompute::IAsync* Compute::Async::m_pIAsync;
	// Public:
	// Constructor/Destructor:
	void Compute::Async::Init(emberBackendInterface::ICompute::IAsync* pIAsync)
	{
		m_pIAsync = pIAsync;
	}
	void Compute::Async::Clear()
	{

	}

	// Dispatch logic:
	uint32_t Compute::Async::CreateComputeSession()
	{
		return m_pIAsync->CreateComputeSession();
	}
	void Compute::Async::DispatchComputeSession(uint32_t sessionID)
	{
		m_pIAsync->DispatchComputeSession(sessionID, Time::GetTime(), Time::GetDeltaTime());
	}
	void Compute::Async::DispatchComputeSessionAndWait(uint32_t sessionID)
	{
		DispatchComputeSession(sessionID);
		WaitForFinish(sessionID);
	}
	bool Compute::Async::IsFinished(uint32_t sessionID)
	{
		return m_pIAsync->IsFinished(sessionID);
	}
	void Compute::Async::WaitForFinish(uint32_t sessionID)
	{
		m_pIAsync->WaitForFinish(sessionID);
	}

	// Workload recording:
	ShaderProperties Compute::Async::RecordComputeShader(uint32_t sessionID, ComputeShader& computeShader, Uint3 threadCount)
	{
		emberBackendInterface::IComputeShader* pIComputeShader = computeShader.GetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pIDescriptorSetBinding = m_pIAsync->RecordComputeShader(sessionID, pIComputeShader, threadCount);
		ShaderProperties shaderProperties = ShaderProperties(pIDescriptorSetBinding);
		return shaderProperties;
	}
	void Compute::Async::RecordBarrier(uint32_t sessionID, ComputeBarrierFlag srcBarrierFlags, ComputeBarrierFlag dstBarrierFlags)
	{
		m_pIAsync->RecordBarrier(sessionID, srcBarrierFlags, dstBarrierFlags);
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
	emberBackendInterface::ICompute::IPostRender* Compute::PostRender::m_pIPostRender;
	// Public:
	// Constructor/Destructor:
	void Compute::PostRender::Init(emberBackendInterface::ICompute::IPostRender* pIPostRender)
	{
		m_pIPostRender = pIPostRender;
	}
	void Compute::PostRender::Clear()
	{

	}

	// Workload recording:
	ShaderProperties Compute::PostRender::RecordComputeShader(ComputeShader& computeShader)
	{
		emberBackendInterface::IComputeShader* pIComputeShader = computeShader.GetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pIComputeCallDescriptorSetBinding = m_pIPostRender->RecordComputeShader(pIComputeShader);
		ShaderProperties shaderProperties = ShaderProperties(pIComputeCallDescriptorSetBinding);
		return shaderProperties;
	}



	// Compute::PreRender subclass:
	emberBackendInterface::ICompute::IPreRender* Compute::PreRender::m_pIPreRender;
	// Public:
	// Constructor/Destructor:
	void Compute::PreRender::Init(emberBackendInterface::ICompute::IPreRender* pIPreRender)
	{
		m_pIPreRender = pIPreRender;
	}
	void Compute::PreRender::Clear()
	{

	}

	// Workload recording:
	ShaderProperties Compute::PreRender::RecordComputeShader(ComputeShader& computeShader, Uint3 threadCount)
	{
		emberBackendInterface::IComputeShader* pIComputeShader = computeShader.GetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pIComputeCallDescriptorSetBinding = m_pIPreRender->RecordComputeShader(pIComputeShader, threadCount);
		ShaderProperties shaderProperties = ShaderProperties(pIComputeCallDescriptorSetBinding);
		return shaderProperties;
	}
	void Compute::PreRender::RecordBarrier(ComputeBarrierFlag srcBarrierFlags, ComputeBarrierFlag dstBarrierFlags)
	{
		m_pIPreRender->RecordBarrier(srcBarrierFlags, dstBarrierFlags);
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
	}
	void Compute::Clear()
	{
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
		}
	}
}