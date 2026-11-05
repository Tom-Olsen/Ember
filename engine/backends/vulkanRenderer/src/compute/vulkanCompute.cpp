#include "vulkanCompute.h"
#include "vulkanAsyncCompute.h"
#include "vulkanComputeQueue.h"
#include "vulkanContext.h"
#include "vulkanSceneColorPair.h"
#include <stdexcept>
#include <utility>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	Compute::Compute()
		: m_submittedQueues(Context::GetFramesInFlight())
	{
		m_pIAsync = std::make_unique<Async>();
		m_pPreRenderComputeQueue = std::make_unique<ComputeQueue>();
		m_pPostOutlineComputeQueue = std::make_unique<ComputeQueue>();
		m_pPostGeometryComputeQueue = std::make_unique<ComputeQueue>();
		m_pPostLightingComputeQueue = std::make_unique<ComputeQueue>(true);
		m_pPostRenderComputeQueue = std::make_unique<ComputeQueue>(true);
	}
	Compute::~Compute()
	{
		if (!Context::IsDeviceIdle())
			Context::WaitDeviceIdle();
		RetireAllFrames();
	}



	// Movable:
	Compute::Compute(Compute&& other) noexcept = default;
	Compute& Compute::operator=(Compute&& other) noexcept
	{
		if (this != &other)
		{
			if (!Context::IsDeviceIdle())
				Context::WaitDeviceIdle();
			RetireAllFrames();
			m_pIAsync = std::move(other.m_pIAsync);
			m_pPreRenderComputeQueue = std::move(other.m_pPreRenderComputeQueue);
			m_pPostOutlineComputeQueue = std::move(other.m_pPostOutlineComputeQueue);
			m_pPostGeometryComputeQueue = std::move(other.m_pPostGeometryComputeQueue);
			m_pPostLightingComputeQueue = std::move(other.m_pPostLightingComputeQueue);
			m_pPostRenderComputeQueue = std::move(other.m_pPostRenderComputeQueue);
			m_submittedQueues = std::move(other.m_submittedQueues);
		}
		return *this;
	}



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
		std::array<ComputeQueue, 5>& submitted = m_submittedQueues.at(frameIndex);
		std::array<ComputeQueue*, 5> queuing = GetActiveQueues();

		// Check if all old commands were executed:
		for (const ComputeQueue& queue : submitted)
			if (!queue.IsEmpty())
				throw std::logic_error("Compute::CommitFrame(...) frame has not been retired.");

		// Retain submitted commands and reset the queues for new work:
		for (uint32_t index = 0; index < queuing.size(); index++)
		{
			submitted[index] = std::move(*queuing[index]);
			submitted[index].Close();
			queuing[index]->Reset();
		}
	}
	void Compute::RetireFrame(uint32_t frameIndex)
	{
		// Retire all submitted queues of active frame:
		for (ComputeQueue& queue : m_submittedQueues.at(frameIndex))
			queue.Reset();
	}
	void Compute::RetireAllFrames()
	{
		// Retire all submitted queues of all frames:
		for (std::array<ComputeQueue, 5>& frame : m_submittedQueues)
			for (ComputeQueue& queue : frame)
				queue.Reset();
	}



	// Private methods:
	std::array<ComputeQueue*, 5> Compute::GetActiveQueues()
	{
		return { m_pPreRenderComputeQueue.get(), m_pPostOutlineComputeQueue.get(), m_pPostGeometryComputeQueue.get(), m_pPostLightingComputeQueue.get(), m_pPostRenderComputeQueue.get() };
	}
}