#include "postRenderEffects.h"
#include "compute.h"
#include "computeShaderManager.h"
#include "shaderProperties.h"
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
		for (ComputeShader* pComputeShader : effects)
			ShaderProperties shaderProperties = Compute::PostRender::RecordComputeShader(*pComputeShader);
	}
}