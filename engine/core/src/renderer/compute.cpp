#include "compute.h"
#include "computeShader.h"
#include "emberTime.h"
#include "shaderProperties.h"
#include "vulkanAsyncCompute.h"
#include "vulkanCompute.h"
#include "vulkanImmediateCompute.h"
#include "vulkanPostRenderCompute.h"
#include "vulkanPreRenderCompute.h"
#include "iCompute.h"



namespace emberEngine
{
	// Compute::Async subclass:
	// Constructor/Destructor:
	Compute::Async::Async(uint16_t sessionCount)
	{
		m_pIAsync = std::make_unique<vulkanRendererBackend::Async>(sessionCount);
	}
	Compute::Async::~Async()
	{
		m_pIAsync.reset();
	}

	// Dispatch logic:
	uint16_t Compute::Async::CreateComputeSession()
	{
		return m_pIAsync->CreateComputeSession();
	}
	void Compute::Async::DispatchComputeSession(uint16_t sessionID)
	{
		m_pIAsync->DispatchComputeSession(sessionID, Time::GetTime(), Time::GetDeltaTime());
	}
	bool Compute::Async::IsFinished(uint16_t sessionID)
	{
		return m_pIAsync->IsFinished(sessionID);
	}
	void Compute::Async::WaitForFinish(uint16_t sessionID)
	{
		m_pIAsync->WaitForFinish(sessionID);
	}

	// Workload recording:
	void Compute::Async::RecordComputeShader(uint16_t sessionID, ComputeShader& computeShader, ShaderProperties& shaderProperties, Uint3 threadCount)
	{
		m_pIAsync->RecordComputeShader(sessionID, computeShader.GetInterfaceHandle(), shaderProperties.GetInterfaceHandle(), threadCount);
	}
	ShaderProperties Compute::Async::RecordComputeShader(uint16_t sessionID, ComputeShader& computeShader, Uint3 threadCount)
	{
		return ShaderProperties(m_pIAsync->RecordComputeShader(sessionID, computeShader.GetInterfaceHandle(), threadCount));
	}
	void Compute::Async::RecordBarrier(uint16_t sessionID, emberCommon::ComputeShaderAccessMask srcAccessMask, emberCommon::ComputeShaderAccessMask dstAccessMask)
	{
		m_pIAsync->RecordBarrier(sessionID, srcAccessMask, dstAccessMask);
	}



	// Compute::Immediate subclass:
	// Constructor/Destructor:
	Compute::Immediate::Immediate()
	{
		m_pIImmediate = std::make_unique<vulkanRendererBackend::Immediate>();
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
	// Constructor/Destructor:
	Compute::PostRender::PostRender()
	{
		m_pIPostRender = std::make_unique<vulkanRendererBackend::PostRender>();
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
	// Constructor/Destructor:
	Compute::PreRender::PreRender()
	{
		m_pIPreRender = std::make_unique<vulkanRendererBackend::PreRender>();
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
	// Initialization/Cleanup:
	void Compute::Init()
	{
		s_pICompute = std::make_unique<vulkanRendererBackend::Compute>();
		s_pAsync = std::make_unique<Async>(10);
		s_pImmediate = std::make_unique<Immediate>();
		s_pPostRender = std::make_unique<PostRender>();
		s_pPreRender = std::make_unique<PreRender>();
	}
	void Compute::Clear()
	{
		s_pICompute.reset();
		s_pAsync.reset();
		s_pImmediate.reset();
		s_pPostRender.reset();
		s_pPreRender.reset();
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