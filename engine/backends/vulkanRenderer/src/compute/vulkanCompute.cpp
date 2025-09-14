#include "vulkanCompute.h"
#include "vulkanAsyncCompute.h"
#include "vulkanImmediateCompute.h"
#include "vulkanPostRenderCompute.h"
#include "vulkanPreRenderCompute.h"



namespace vulkanRendererBackend
{
	// Static members:
	bool Compute::s_isInitialized = false;



	// Initialization/Cleanup:
	void Compute::Init()
	{
		if (s_isInitialized)
			return;
		Async::Init(10);
		Immediate::Init();
		PostRender::Init();
		PreRender::Init();
	}
	void Compute::Clear()
	{
		PreRender::Clear();
		PostRender::Clear();
		Immediate::Clear();
		Async::Clear();
	}
}