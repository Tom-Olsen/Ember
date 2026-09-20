#include "vulkanCompute.h"
#include "vulkanAsyncCompute.h"
#include "vulkanComputeQueue.h"
#include "vulkanSceneColorTexture2dPair.h"



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	Compute::Compute()
	{
		uint32_t asyncSessionCount = 10;
		m_pIAsync = std::make_unique<Async>(asyncSessionCount);
		m_pPreRenderComputeQueue = std::make_unique<ComputeQueue>();
		m_pMidRenderComputeQueue = std::make_unique<ComputeQueue>();
		m_pScreenSpaceComputeQueue = std::make_unique<ComputeQueue>(true);
		m_pPostRenderComputeQueue = std::make_unique<ComputeQueue>(true);
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
	ComputeQueue* Compute::GetPreRenderCompute()
	{
		return m_pPreRenderComputeQueue.get();
	}
	ComputeQueue* Compute::GetMidRenderCompute()
	{
		return m_pMidRenderComputeQueue.get();
	}
	ComputeQueue* Compute::GetScreenSpaceCompute()
	{
		return m_pScreenSpaceComputeQueue.get();
	}
	ComputeQueue* Compute::GetPostRenderCompute()
	{
		return m_pPostRenderComputeQueue.get();
	}
	emberBackendInterface::ICompute::IAsync* Compute::GetAsyncComputeInterfaceHandle()
	{
		return static_cast<emberBackendInterface::ICompute::IAsync*>(m_pIAsync.get());
	}
	emberBackendInterface::ICompute::IQueue* Compute::GetPreRenderComputeInterfaceHandle()
	{
		return m_pPreRenderComputeQueue.get();
	}
	emberBackendInterface::ICompute::IQueue* Compute::GetMidRenderComputeInterfaceHandle()
	{
		return m_pMidRenderComputeQueue.get();
	}
	emberBackendInterface::ICompute::IQueue* Compute::GetScreenSpaceComputeInterfaceHandle()
	{
		return m_pScreenSpaceComputeQueue.get();
	}
	emberBackendInterface::ICompute::IQueue* Compute::GetPostRenderComputeInterfaceHandle()
	{
		return m_pPostRenderComputeQueue.get();
	}



	// Frame lifecycle:
	uint32_t Compute::UpdateShaderData(uint32_t frameIndex, SceneColorTexture2dPair& sceneColorTexturePair)
	{
		GetPreRenderCompute()->UpdateShaderData(frameIndex);
		GetMidRenderCompute()->UpdateShaderData(frameIndex);
		GetScreenSpaceCompute()->UpdateShaderData(frameIndex, &sceneColorTexturePair);
		uint32_t sceneColorTextureIndex = sceneColorTexturePair.GetCurrentTextureIndex(frameIndex);
		GetPostRenderCompute()->UpdateShaderData(frameIndex, &sceneColorTexturePair);
		return sceneColorTextureIndex;
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