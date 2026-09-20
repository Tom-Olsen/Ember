#include "postRenderEffects.h"
#include "callProperties.h"
#include "compute.h"
#include "computeShaderManager.h"
using namespace emberCore;



namespace emberEcs
{
	// Constructor/Destructor:
	PostRenderEffects::PostRenderEffects()
	{
		effects.push_back(ComputeShaderManager::TryGetComputeShader("postProcessing"));
	}
	PostRenderEffects::~PostRenderEffects()
	{
		effects.clear();
	}



	// Overrides:
	void PostRenderEffects::LateUpdate()
	{
		for (ComputeShader& computeShader : effects)
			CallProperties callProperties = Compute::PostRender::RecordPostProcessingShader(computeShader);
	}
}