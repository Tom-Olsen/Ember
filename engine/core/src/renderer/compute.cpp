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
		emberBackendInterface::IComputeShader* pIComputeShader = computeShader.GetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pIDescriptorSetBinding = shaderProperties.GetCallInterfaceHandle();
		if (!pIDescriptorSetBinding)
		{
			LOG_WARN("Compute::Async::RecordComputeShader(...) skipped stale ShaderProperties. Reassign ShaderProperties before reusing it for another compute call.");
			return;
		}
		m_pIAsync->RecordComputeShader(sessionID, pIComputeShader, pIDescriptorSetBinding, threadCount);
	}
	ShaderProperties Compute::Async::RecordComputeShader(uint32_t sessionID, ComputeShader& computeShader, Uint3 threadCount)
	{
		emberBackendInterface::IComputeShader* pIComputeShader = computeShader.GetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pIDescriptorSetBinding = m_pIAsync->RecordComputeShader(sessionID, pIComputeShader, threadCount);
		ShaderProperties shaderProperties = ShaderProperties(computeShader, pIDescriptorSetBinding, false);
		return shaderProperties;
	}
	void Compute::Async::RecordBarrier(uint32_t sessionID, emberCommon::ComputeAccessFlag srcAccessMask, emberCommon::ComputeAccessFlag dstAccessMask)
	{
		m_pIAsync->RecordBarrier(sessionID, srcAccessMask, dstAccessMask);
	}
	void Compute::Async::RecordBarrierWaitStorageWriteBeforeRead(uint32_t sessionID)
	{
		Compute::Async::RecordBarrier(sessionID, emberCommon::ComputeAccessFlag::storageWrite, emberCommon::ComputeAccessFlag::storageRead);
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
		emberBackendInterface::IComputeShader* pIComputeShader = computeShader.GetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pIDescriptorSetBinding = shaderProperties.GetCallInterfaceHandle();
		if (!pIDescriptorSetBinding)
		{
			LOG_WARN("Compute::Immediate::Dispatch(...) skipped stale ShaderProperties. Reassign ShaderProperties before reusing it for another compute call.");
			return;
		}
		m_pIImmediate->Dispatch(pIComputeShader, pIDescriptorSetBinding, threadCount, Time::GetTime(), Time::GetDeltaTime());
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
		emberBackendInterface::IComputeShader* pIComputeShader = computeShader.GetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pIDescriptorSetBinding = shaderProperties.GetCallInterfaceHandle();
		if (!pIDescriptorSetBinding)
		{
			LOG_WARN("Compute::PostRender::RecordComputeShader(...) skipped stale ShaderProperties. Reassign ShaderProperties before reusing it for another compute call.");
			return;
		}
		m_pIPostRender->RecordComputeShader(pIComputeShader, pIDescriptorSetBinding);
	}
	ShaderProperties Compute::PostRender::RecordComputeShader(ComputeShader& computeShader)
	{
		emberBackendInterface::IComputeShader* pIComputeShader = computeShader.GetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pIComputeCallDescriptorSetBinding = m_pIPostRender->RecordComputeShader(pIComputeShader);
		ShaderProperties shaderProperties = ShaderProperties(computeShader, pIComputeCallDescriptorSetBinding, false);
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
		emberBackendInterface::IComputeShader* pIComputeShader = computeShader.GetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pIDescriptorSetBinding = shaderProperties.GetCallInterfaceHandle();
		if (!pIDescriptorSetBinding)
		{
			LOG_WARN("Compute::PreRender::RecordComputeShader(...) skipped stale ShaderProperties. Reassign ShaderProperties before reusing it for another compute call.");
			return;
		}
		m_pIPreRender->RecordComputeShader(pIComputeShader, pIDescriptorSetBinding, threadCount);
	}
	ShaderProperties Compute::PreRender::RecordComputeShader(ComputeShader& computeShader, Uint3 threadCount)
	{
		emberBackendInterface::IComputeShader* pIComputeShader = computeShader.GetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pIComputeCallDescriptorSetBinding = m_pIPreRender->RecordComputeShader(pIComputeShader, threadCount);
		ShaderProperties shaderProperties = ShaderProperties(computeShader, pIComputeCallDescriptorSetBinding, false);
		return shaderProperties;
	}
	void Compute::PreRender::RecordBarrier(emberCommon::ComputeAccessFlag srcAccessMask, emberCommon::ComputeAccessFlag dstAccessMask)
	{
		m_pIPreRender->RecordBarrier(srcAccessMask, dstAccessMask);
	}
	void Compute::PreRender::RecordBarrierWaitStorageWriteBeforeRead()
	{
		Compute::PreRender::RecordBarrier(emberCommon::ComputeAccessFlag::storageWrite, emberCommon::ComputeAccessFlag::storageRead);
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
				// Post render compute is render-target sized by design, so threadCount is ignored here.
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
				throw std::runtime_error("Compute::RecordComputeShader: immediate compute requires explicit ShaderProperties.");
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
	void Compute::RecordBarrier(ComputeType computeType, emberCommon::ComputeAccessFlag srcAccessMask, emberCommon::ComputeAccessFlag dstAccessMask , uint32_t sessionID)
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