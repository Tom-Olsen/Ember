#include "vulkanCompute.h"
#include "vulkanAsyncCompute.h"
#include "vulkanContext.h"
#include "vulkanMidRenderCompute.h"
#include "vulkanPostRenderCompute.h"
#include "vulkanPreRenderCompute.h"
#include "vulkanSceneColorTexture2dPair.h"
#include "vulkanScreenSpaceCompute.h"



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	Compute::Compute()
	{
		m_pIAsync = std::make_unique<Async>(10);	// 10 = max session count.
		m_pIPreRender = std::make_unique<PreRender>();
		m_pIMidRender = std::make_unique<MidRender>();
		m_pIScreenSpace = std::make_unique<ScreenSpace>();
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
	MidRender* Compute::GetMidRenderCompute()
	{
		return static_cast<MidRender*>(m_pIMidRender.get());
	}
	ScreenSpace* Compute::GetScreenSpaceCompute()
	{
		return static_cast<ScreenSpace*>(m_pIScreenSpace.get());
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
	emberBackendInterface::ICompute::IMidRender* Compute::GetMidRenderComputeInterfaceHandle()
	{
		return static_cast<emberBackendInterface::ICompute::IMidRender*>(m_pIMidRender.get());
	}
	emberBackendInterface::ICompute::IScreenSpace* Compute::GetScreenSpaceComputeInterfaceHandle()
	{
		return static_cast<emberBackendInterface::ICompute::IScreenSpace*>(m_pIScreenSpace.get());
	}
	emberBackendInterface::ICompute::IPostRender* Compute::GetPostRenderComputeInterfaceHandle()
	{
		return static_cast<emberBackendInterface::ICompute::IPostRender*>(m_pIPostRender.get());
	}



	// Frame lifecycle:
	void Compute::UpdateShaderData(uint32_t frameIndex, SceneColorTexture2dPair& sceneColorTexturePair)
	{
		GetPreRenderCompute()->UpdateShaderData(frameIndex);
		GetMidRenderCompute()->UpdateShaderData(frameIndex);
		GetScreenSpaceCompute()->UpdateShaderData(frameIndex);
		GetPostRenderCompute()->UpdateShaderData(frameIndex, sceneColorTexturePair);
	}
	void Compute::CommitFrame(uint32_t frameIndex)
	{
		GetPreRenderCompute()->CommitComputeCalls(frameIndex);
		GetMidRenderCompute()->CommitComputeCalls(frameIndex);
		GetScreenSpaceCompute()->CommitComputeCalls(frameIndex);
		GetPostRenderCompute()->CommitComputeCalls(frameIndex);
	}
	void Compute::RetireFrame(uint32_t frameIndex)
	{
		GetPreRenderCompute()->RetireComputeCalls(frameIndex);
		GetMidRenderCompute()->RetireComputeCalls(frameIndex);
		GetScreenSpaceCompute()->RetireComputeCalls(frameIndex);
		GetPostRenderCompute()->RetireComputeCalls(frameIndex);
	}
	void Compute::RetireAllFrames()
	{
		GetPreRenderCompute()->RetireAllComputeCalls();
		GetMidRenderCompute()->RetireAllComputeCalls();
		GetScreenSpaceCompute()->RetireAllComputeCalls();
		GetPostRenderCompute()->RetireAllComputeCalls();
	}
	void Compute::DiscardPendingCalls()
	{
		GetPreRenderCompute()->ResetComputeCalls();
		GetMidRenderCompute()->ResetComputeCalls();
		GetScreenSpaceCompute()->ResetComputeCalls();
		GetPostRenderCompute()->ResetComputeCalls();
	}
}