#include "vulkanCompute.h"
#include "vulkanContext.h"
#include "vulkanAsyncCompute.h"
#include "vulkanImmediateCompute.h"
#include "vulkanPostRenderCompute.h"
#include "vulkanPreRenderCompute.h"



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	Compute::Compute()
	{
		m_pIAsync = std::make_unique<Async>(10);
		m_pIImmediate = std::make_unique<Immediate>();
		m_pIPostRender = std::make_unique<PostRender>();
		m_pIPreRender = std::make_unique<PreRender>();
	}
	Compute::~Compute()
	{
		Context::Clear();
	}



	// Movable:
	Compute::Compute(Compute&& other) noexcept = default;
	Compute& Compute::operator=(Compute&& other) noexcept = default;



	// Getters:
	Async* Compute::GetAsyncCompute()
	{
		return static_cast<Async*>(m_pIAsync.get());
	}
	Immediate* Compute::GetImmediateCompute()
	{
		return static_cast<Immediate*>(m_pIImmediate.get());
	}
	PostRender* Compute::GetPostRenderCompute()
	{
		return static_cast<PostRender*>(m_pIPostRender.get());
	}
	PreRender* Compute::GetPreRenderCompute()
	{
		return static_cast<PreRender*>(m_pIPreRender.get());
	}
}