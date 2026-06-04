#include "vulkanCompute.h"
#include "vulkanContext.h"
#include "vulkanAsyncCompute.h"
#include "vulkanPostRenderCompute.h"
#include "vulkanPreRenderCompute.h"



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	Compute::Compute()
	{
		m_pIAsync = std::make_unique<Async>(10);	// 10 = max session count.
		m_pIPostRender = std::make_unique<PostRender>();
		m_pIPreRender = std::make_unique<PreRender>();
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
	PostRender* Compute::GetPostRenderCompute()
	{
		return static_cast<PostRender*>(m_pIPostRender.get());
	}
	PreRender* Compute::GetPreRenderCompute()
	{
		return static_cast<PreRender*>(m_pIPreRender.get());
	}
	emberBackendInterface::ICompute::IAsync* Compute::GetAsyncComputeInterfaceHandle()
	{
		return static_cast<emberBackendInterface::ICompute::IAsync*>(m_pIAsync.get());
	}
	emberBackendInterface::ICompute::IPostRender* Compute::GetPostRenderComputeInterfaceHandle()
	{
		return static_cast<emberBackendInterface::ICompute::IPostRender*>(m_pIPostRender.get());
	}
	emberBackendInterface::ICompute::IPreRender* Compute::GetPreRenderComputeInterfaceHandle()
	{
		return static_cast<emberBackendInterface::ICompute::IPreRender*>(m_pIPreRender.get());
	}
}