#include "postRenderEffects.h"



namespace emberEngine
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
			ShaderProperties shaderProperties = Compute::GetPostRender()->RecordComputeShader(*pComputeShader);
	}
	const std::string PostRenderEffects::ToString() const
	{
		return "PostRenderEffects";
	}
}