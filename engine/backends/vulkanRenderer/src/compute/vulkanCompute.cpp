#include "vulkanCompute.h"
#include "vulkanAsyncCompute.h"
#include "vulkanComputeQueue.h"
#include "vulkanSceneColorPair.h"



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	Compute::Compute()
	{
		m_pIAsync = std::make_unique<Async>();
		m_pPreRenderComputeQueue = std::make_unique<ComputeQueue>();
		m_pPostOutlineComputeQueue = std::make_unique<ComputeQueue>();
		m_pPostGeometryComputeQueue = std::make_unique<ComputeQueue>();
		m_pPostLightingComputeQueue = std::make_unique<ComputeQueue>();
		m_pPostRenderComputeQueue = std::make_unique<ComputeQueue>();
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
	ComputeQueue* Compute::GetPostOutlineCompute()
	{
		return m_pPostOutlineComputeQueue.get();
	}
	ComputeQueue* Compute::GetPostGeometryCompute()
	{
		return m_pPostGeometryComputeQueue.get();
	}
	ComputeQueue* Compute::GetPostLightingCompute()
	{
		return m_pPostLightingComputeQueue.get();
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
	emberBackendInterface::ICompute::IQueue* Compute::GetPostOutlineComputeInterfaceHandle()
	{
		return m_pPostOutlineComputeQueue.get();
	}
	emberBackendInterface::ICompute::IQueue* Compute::GetPostGeometryComputeInterfaceHandle()
	{
		return m_pPostGeometryComputeQueue.get();
	}
	emberBackendInterface::ICompute::IQueue* Compute::GetPostLightingComputeInterfaceHandle()
	{
		return m_pPostLightingComputeQueue.get();
	}
	emberBackendInterface::ICompute::IQueue* Compute::GetPostRenderComputeInterfaceHandle()
	{
		return m_pPostRenderComputeQueue.get();
	}



	// Frame lifecycle:
	uint32_t Compute::UpdateShaderData(uint32_t frameIndex, SceneColorPair& sceneColorPair)
	{
		GetPreRenderCompute()->UpdateShaderData(frameIndex);
		GetPostOutlineCompute()->UpdateShaderData(frameIndex);
		GetPostGeometryCompute()->UpdateShaderData(frameIndex);
		GetPostLightingCompute()->UpdateShaderData(frameIndex, &sceneColorPair);
		uint32_t sceneColorTextureIndex = sceneColorPair.GetCurrentTextureIndex(frameIndex);
		GetPostRenderCompute()->UpdateShaderData(frameIndex, &sceneColorPair);
		return sceneColorTextureIndex;
	}
	void Compute::CommitFrame(uint32_t frameIndex)
	{
		GetPreRenderCompute()->CommitComputeCalls(frameIndex);
		GetPostOutlineCompute()->CommitComputeCalls(frameIndex);
		GetPostGeometryCompute()->CommitComputeCalls(frameIndex);
		GetPostLightingCompute()->CommitComputeCalls(frameIndex);
		GetPostRenderCompute()->CommitComputeCalls(frameIndex);
	}
	void Compute::RetireFrame(uint32_t frameIndex)
	{
		GetPreRenderCompute()->RetireComputeCalls(frameIndex);
		GetPostOutlineCompute()->RetireComputeCalls(frameIndex);
		GetPostGeometryCompute()->RetireComputeCalls(frameIndex);
		GetPostLightingCompute()->RetireComputeCalls(frameIndex);
		GetPostRenderCompute()->RetireComputeCalls(frameIndex);
	}
	void Compute::RetireAllFrames()
	{
		GetPreRenderCompute()->RetireAllComputeCalls();
		GetPostOutlineCompute()->RetireAllComputeCalls();
		GetPostGeometryCompute()->RetireAllComputeCalls();
		GetPostLightingCompute()->RetireAllComputeCalls();
		GetPostRenderCompute()->RetireAllComputeCalls();
	}
	void Compute::DiscardPendingCalls()
	{
		GetPreRenderCompute()->ResetComputeCalls();
		GetPostOutlineCompute()->ResetComputeCalls();
		GetPostGeometryCompute()->ResetComputeCalls();
		GetPostLightingCompute()->ResetComputeCalls();
		GetPostRenderCompute()->ResetComputeCalls();
	}
}