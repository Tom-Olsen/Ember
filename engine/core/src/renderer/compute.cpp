#include "compute.h"
#include "computeShader.h"
#include "emberTime.h"
#include "shaderProperties.h"
#include "iCompute.h"



namespace emberEngine
{
	// Compute::Async subclass:
	// Public:
	// Constructor/Destructor:
	Compute::Async::Async(emberBackendInterface::ICompute::IAsync* pIAsync)
	{
		m_pIAsync = std::unique_ptr<emberBackendInterface::ICompute::IAsync>(pIAsync);
	}
	Compute::Async::~Async()
	{
		m_pIAsync.reset();
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
		return ShaderProperties(m_pIAsync->RecordComputeShader(sessionID, computeShader.GetInterfaceHandle(), threadCount));
	}
	void Compute::Async::RecordBarrier(uint32_t sessionID, emberCommon::ComputeShaderAccessMask srcAccessMask, emberCommon::ComputeShaderAccessMask dstAccessMask)
	{
		m_pIAsync->RecordBarrier(sessionID, srcAccessMask, dstAccessMask);
	}



	// Compute::Immediate subclass:
	// Public:
	// Constructor/Destructor:
	Compute::Immediate::Immediate(emberBackendInterface::ICompute::IImmediate* pIImmediate)
	{
		m_pIImmediate = std::unique_ptr<emberBackendInterface::ICompute::IImmediate>(pIImmediate);
	}
	Compute::Immediate::~Immediate()
	{
		m_pIImmediate.reset();
	}

	// Immediate dispatch call:
	void Compute::Immediate::Dispatch(ComputeShader& computeShader, ShaderProperties& shaderProperties, Uint3 threadCount)
	{
		m_pIImmediate->Dispatch(computeShader.GetInterfaceHandle(), shaderProperties.GetInterfaceHandle(), threadCount, Time::GetTime(), Time::GetDeltaTime());
	}



	// Compute::PostRender subclass:
	// Public:
	// Constructor/Destructor:
	Compute::PostRender::PostRender(emberBackendInterface::ICompute::IPostRender* pIPostRender)
	{
		m_pIPostRender = std::unique_ptr<emberBackendInterface::ICompute::IPostRender>(pIPostRender);
	}
	Compute::PostRender::~PostRender()
	{
		m_pIPostRender.reset();
	}

	// Workload recording:
	void Compute::PostRender::RecordComputeShader(ComputeShader& computeShader, ShaderProperties& shaderProperties)
	{
		m_pIPostRender->RecordComputeShader(computeShader.GetInterfaceHandle(), shaderProperties.GetInterfaceHandle());
	}
	ShaderProperties Compute::PostRender::RecordComputeShader(ComputeShader& computeShader)
	{
		return ShaderProperties(m_pIPostRender->RecordComputeShader(computeShader.GetInterfaceHandle()));
	}



	// Compute::PreRender subclass:
	// Public:
	// Constructor/Destructor:
	Compute::PreRender::PreRender(emberBackendInterface::ICompute::IPreRender* pIPreRender)
	{
		m_pIPreRender = std::unique_ptr<emberBackendInterface::ICompute::IPreRender>(pIPreRender);
	}
	Compute::PreRender::~PreRender()
	{
		m_pIPreRender.reset();
	}

	// Workload recording:
	void Compute::PreRender::RecordComputeShader(ComputeShader& computeShader, ShaderProperties& shaderProperties, Uint3 threadCount)
	{
		m_pIPreRender->RecordComputeShader(computeShader.GetInterfaceHandle(), shaderProperties.GetInterfaceHandle(), threadCount);
	}
	ShaderProperties Compute::PreRender::RecordComputeShader(ComputeShader& computeShader, Uint3 threadCount)
	{
		return ShaderProperties(m_pIPreRender->RecordComputeShader(computeShader.GetInterfaceHandle(), threadCount));
	}
	void Compute::PreRender::RecordBarrier(emberCommon::ComputeShaderAccessMask srcAccessMask, emberCommon::ComputeShaderAccessMask dstAccessMask)
	{
		m_pIPreRender->RecordBarrier(srcAccessMask, dstAccessMask);
	}



	// Compute class:
	// Static members:
	bool Compute::s_isInitialized = false;
	std::unique_ptr<emberBackendInterface::ICompute> Compute::s_pICompute;
	std::unique_ptr<Compute::Async> Compute::s_pAsync;
	std::unique_ptr<Compute::Immediate> Compute::s_pImmediate;
	std::unique_ptr<Compute::PostRender> Compute::s_pPostRender;
	std::unique_ptr<Compute::PreRender> Compute::s_pPreRender;



	// Public:
	// Initialization/Cleanup:
	void Compute::Init(emberBackendInterface::ICompute* pICompute)
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		s_pICompute = std::unique_ptr<emberBackendInterface::ICompute>(pICompute);
		s_pAsync = std::make_unique<Async>(s_pICompute->GetAsyncComputeInterfaceHandle());
		s_pImmediate = std::make_unique<Immediate>(s_pICompute->GetImmediateComputeInterfaceHandle());
		s_pPostRender = std::make_unique<PostRender>(s_pICompute->GetPostRenderComputeInterfaceHandle());
		s_pPreRender = std::make_unique<PreRender>(s_pICompute->GetPreRenderComputeInterfaceHandle());
	}
	void Compute::Clear()
	{
		s_pICompute.reset();
		s_pAsync.reset();
		s_pImmediate.reset();
		s_pPostRender.reset();
		s_pPreRender.reset();
		s_isInitialized = false;
	}

	// Getters:
	Compute::Async* Compute::GetAsync()
	{
		return s_pAsync.get();
	}
	Compute::Immediate* Compute::GetImmediate()
	{
		return s_pImmediate.get();
	}
	Compute::PostRender* Compute::GetPostRender()
	{
		return s_pPostRender.get();
	}
	Compute::PreRender* Compute::GetPreRender()
	{
		return s_pPreRender.get();
	}
}