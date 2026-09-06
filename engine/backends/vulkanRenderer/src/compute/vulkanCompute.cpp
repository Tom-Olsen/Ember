#include "vulkanCompute.h"
#include "vulkanAsyncCompute.h"
#include "vulkanContext.h"
#include "vulkanPostRenderCompute.h"
#include "vulkanPreRenderCompute.h"
#include "vulkanRenderCompute.h"
#include "vulkanSceneColorTexture2dPair.h"



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	Compute::Compute()
	{
		m_pIAsync = std::make_unique<Async>(10);	// 10 = max session count.
		m_pIPreRender = std::make_unique<PreRender>();
		m_pIRender = std::make_unique<Render>();
		m_pIPostRender = std::make_unique<PostRender>();
	}
	Compute::~Compute()
	{

	}



	// Movable:
	Compute::Compute(Compute&& other) noexcept = default;
	Compute& Compute::operator=(Compute&& other) noexcept = default;



	// Getters:
	Async* Compute::GetAsyncCompute()
	{
		return static_cast<Async*>(m_pIAsync.get());
	}
	PreRender* Compute::GetPreRenderCompute()
	{
		return static_cast<PreRender*>(m_pIPreRender.get());
	}
	Render* Compute::GetRenderCompute()
	{
		return static_cast<Render*>(m_pIRender.get());
	}
	PostRender* Compute::GetPostRenderCompute()
	{
		return static_cast<PostRender*>(m_pIPostRender.get());
	}
	emberBackendInterface::ICompute::IAsync* Compute::GetAsyncComputeInterfaceHandle()
	{
		return static_cast<emberBackendInterface::ICompute::IAsync*>(m_pIAsync.get());
	}
	emberBackendInterface::ICompute::IPreRender* Compute::GetPreRenderComputeInterfaceHandle()
	{
		return static_cast<emberBackendInterface::ICompute::IPreRender*>(m_pIPreRender.get());
	}
	emberBackendInterface::ICompute::IRender* Compute::GetRenderComputeInterfaceHandle()
	{
		return static_cast<emberBackendInterface::ICompute::IRender*>(m_pIRender.get());
	}
	emberBackendInterface::ICompute::IPostRender* Compute::GetPostRenderComputeInterfaceHandle()
	{
		return static_cast<emberBackendInterface::ICompute::IPostRender*>(m_pIPostRender.get());
	}



	// Frame lifecycle:
	void Compute::UpdateShaderData(uint32_t frameIndex, SceneColorTexture2dPair& sceneColorTexturePair)
	{
		GetPreRenderCompute()->UpdateShaderData(frameIndex);
		GetRenderCompute()->UpdateShaderData(frameIndex);
		GetPostRenderCompute()->UpdateShaderData(frameIndex, sceneColorTexturePair);
	}
	void Compute::CommitFrame(uint32_t frameIndex)
	{
		GetPreRenderCompute()->CommitComputeCalls(frameIndex);
		GetRenderCompute()->CommitComputeCalls(frameIndex);
		GetPostRenderCompute()->CommitComputeCalls(frameIndex);
	}
	void Compute::RetireFrame(uint32_t frameIndex)
	{
		GetPreRenderCompute()->RetireComputeCalls(frameIndex);
		GetRenderCompute()->RetireComputeCalls(frameIndex);
		GetPostRenderCompute()->RetireComputeCalls(frameIndex);
	}
	void Compute::RetireAllFrames()
	{
		GetPreRenderCompute()->RetireAllComputeCalls();
		GetRenderCompute()->RetireAllComputeCalls();
		GetPostRenderCompute()->RetireAllComputeCalls();
	}
	void Compute::DiscardPendingCalls()
	{
		GetPreRenderCompute()->ResetComputeCalls();
		GetRenderCompute()->ResetComputeCalls();
		GetPostRenderCompute()->ResetComputeCalls();
	}
}