#include "compute.h"
#include "logger.h"
#include "macros.h"



namespace emberEngine
{
	// Static members:
	bool Compute::s_isInitialized = false;



	// Initialization/Cleanup:
	void Compute::Init()
	{
		if (s_isInitialized)
			return;
		compute::Async::Init(10);
		compute::Immediate::Init();
		compute::PostRender::Init();
		compute::PreRender::Init();

		#ifdef LOG_INITIALIZATION
		LOG_TRACE("Compute initialized.");
		#endif
	}
	void Compute::Clear()
	{
		compute::PreRender::Clear();
		compute::PostRender::Clear();
		compute::Immediate::Clear();
		compute::Async::Clear();
	}
}