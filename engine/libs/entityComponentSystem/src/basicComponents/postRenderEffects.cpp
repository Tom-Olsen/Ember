#include "postRenderEffects.h"
#include "callProperties.h"
#include "compute.h"
#include "computeShaderManager.h"
#include "renderer.h"
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
		Uint3 threadCount = Uint3(emberCore::Renderer::GetRenderWidth(), emberCore::Renderer::GetRenderHeight(), 1);
		for (ComputeShader& computeShader : effects)
			CallProperties callProperties = Compute::PostRender::DispatchPostProcessingShader(computeShader, threadCount);
	}
}