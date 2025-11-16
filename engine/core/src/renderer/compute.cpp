#include "compute.h"
#include "computeShader.h"
#include "emberTime.h"
#include "logger.h"
#include "shaderProperties.h"
#include "iCompute.h"



namespace emberEngine
{
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
	bool Compute::Async::IsFinished(uint32_t sessionID)
	{
		return m_pIAsync->IsFinished(sessionID);
	}
	void Compute::Async::WaitForFinish(uint32_t sessionID)
	{
		m_pIAsync->WaitForFinish(sessionID);
	}

	// Workload recording:
	void Compute::Async::RecordComputeShader(uint32_t sessionID, ComputeShader& computeShader, ShaderProperties& shaderProperties, Uint3 threadCount)
	{
		m_pIAsync->RecordComputeShader(sessionID, computeShader.GetInterfaceHandle(), shaderProperties.GetInterfaceHandle(), threadCount);
	}
	ShaderProperties Compute::Async::RecordComputeShader(uint32_t sessionID, ComputeShader& computeShader, Uint3 threadCount)
	{
		ShaderProperties shaderProperties = ShaderProperties(m_pIAsync->RecordComputeShader(sessionID, computeShader.GetInterfaceHandle(), threadCount));
		shaderProperties.SetOwnerShip(false);
		return shaderProperties;
	}
	void Compute::Async::RecordBarrier(uint32_t sessionID, emberCommon::ComputeShaderAccessMask srcAccessMask, emberCommon::ComputeShaderAccessMask dstAccessMask)
	{
		m_pIAsync->RecordBarrier(sessionID, srcAccessMask, dstAccessMask);
	}
	void Compute::Async::RecordBarrierWaitStorageWriteBeforeRead(uint32_t sessionID)
	{
		Compute::Async::RecordBarrier(sessionID, emberCommon::AccessMasks::computeShader_storageWrite, emberCommon::AccessMasks::computeShader_storageRead);
	}



	// Compute::Immediate subclass:
	emberBackendInterface::ICompute::IImmediate* Compute::Immediate::m_pIImmediate;
	// Public:
	// Constructor/Destructor:
	void Compute::Immediate::Init(emberBackendInterface::ICompute::IImmediate* pIImmediate)
	{
		m_pIImmediate = pIImmediate;
	}
	void Compute::Immediate::Clear()
	{

	}

	// Immediate dispatch call:
	void Compute::Immediate::Dispatch(ComputeShader& computeShader, ShaderProperties& shaderProperties, Uint3 threadCount)
	{
		m_pIImmediate->Dispatch(computeShader.GetInterfaceHandle(), shaderProperties.GetInterfaceHandle(), threadCount, Time::GetTime(), Time::GetDeltaTime());
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
	void Compute::PostRender::RecordComputeShader(ComputeShader& computeShader, ShaderProperties& shaderProperties)
	{
		m_pIPostRender->RecordComputeShader(computeShader.GetInterfaceHandle(), shaderProperties.GetInterfaceHandle());
	}
	ShaderProperties Compute::PostRender::RecordComputeShader(ComputeShader& computeShader)
	{
		ShaderProperties shaderProperties = ShaderProperties(m_pIPostRender->RecordComputeShader(computeShader.GetInterfaceHandle()));
		shaderProperties.SetOwnerShip(false);
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
	void Compute::PreRender::RecordComputeShader(ComputeShader& computeShader, ShaderProperties& shaderProperties, Uint3 threadCount)
	{
		m_pIPreRender->RecordComputeShader(computeShader.GetInterfaceHandle(), shaderProperties.GetInterfaceHandle(), threadCount);
	}
	ShaderProperties Compute::PreRender::RecordComputeShader(ComputeShader& computeShader, Uint3 threadCount)
	{
		ShaderProperties shaderProperties = ShaderProperties(m_pIPreRender->RecordComputeShader(computeShader.GetInterfaceHandle(), threadCount));
		shaderProperties.SetOwnerShip(false);
		return shaderProperties;
	}
	void Compute::PreRender::RecordBarrier(emberCommon::ComputeShaderAccessMask srcAccessMask, emberCommon::ComputeShaderAccessMask dstAccessMask)
	{
		m_pIPreRender->RecordBarrier(srcAccessMask, dstAccessMask);
	}
	void Compute::PreRender::RecordBarrierWaitStorageWriteBeforeRead()
	{
		Compute::PreRender::RecordBarrier(emberCommon::AccessMasks::computeShader_storageWrite, emberCommon::AccessMasks::computeShader_storageRead);
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
		Compute::Immediate::Init(s_pICompute->GetImmediateComputeInterfaceHandle());
		Compute::PostRender::Init(s_pICompute->GetPostRenderComputeInterfaceHandle());
		Compute::PreRender::Init(s_pICompute->GetPreRenderComputeInterfaceHandle());
	}
	void Compute::Clear()
	{
		Compute::PreRender::Clear();
		Compute::PostRender::Clear();
		Compute::Immediate::Clear();
		Compute::Async::Clear();
		s_pICompute.reset();
		s_isInitialized = false;
	}



	// Workload recording:
	void Compute::RecordComputeShader(ComputeType computeType, ComputeShader& computeShader, ShaderProperties& shaderProperties, Uint3 threadCount, uint32_t sessionID)
	{
		switch (computeType)
		{
			case ComputeType::async:
				Async::RecordComputeShader(sessionID, computeShader, shaderProperties, threadCount);
				break;
			case ComputeType::immediate:
				Immediate::Dispatch(computeShader, shaderProperties, threadCount);
				break;
			case ComputeType::postRender:
				PostRender::RecordComputeShader(computeShader, shaderProperties);
				break;
			case ComputeType::preRender:
				PreRender::RecordComputeShader(computeShader, shaderProperties, threadCount);
				break;
		}
	}
	ShaderProperties Compute::RecordComputeShader(ComputeType computeType, ComputeShader& computeShader, Uint3 threadCount, uint32_t sessionID)
	{
		switch (computeType)
		{
			case ComputeType::async:
				return Async::RecordComputeShader(sessionID, computeShader, threadCount);
				break;
			case ComputeType::immediate:
				break;
			case ComputeType::postRender:
				return PostRender::RecordComputeShader(computeShader);
				break;
			case ComputeType::preRender:
				return PreRender::RecordComputeShader(computeShader, threadCount);
				break;
		}
	}
	void Compute::RecordBarrier(ComputeType computeType, emberCommon::ComputeShaderAccessMask srcAccessMask, emberCommon::ComputeShaderAccessMask dstAccessMask , uint32_t sessionID)
	{
		switch (computeType)
		{
			case ComputeType::async:
				Async::RecordBarrier(sessionID, srcAccessMask, dstAccessMask);
				break;
			case ComputeType::immediate:
				break;
			case ComputeType::postRender:
				break;
			case ComputeType::preRender:
				PreRender::RecordBarrier(srcAccessMask, dstAccessMask);
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
			case ComputeType::immediate:
				break;
			case ComputeType::postRender:
				break;
			case ComputeType::preRender:
				PreRender::RecordBarrierWaitStorageWriteBeforeRead();
				break;
		}
	}
}