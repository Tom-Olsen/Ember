#include "compute.h"
#include "asyncCompute.h"
#include "immediateCompute.h"
#include "postRenderCompute.h"
#include "preRenderCompute.h"



namespace vulkanRendererBackend
{
	// Static members:
	bool Compute::s_isInitialized = false;



	// Initialization/Cleanup:
	void Compute::Init(const std::filesystem::path& inOutComputeShaderSpv)
	{
		if (s_isInitialized)
			return;
		Async::Init(10);
		Immediate::Init();
		PostRender::Init(inOutComputeShaderSpv);
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