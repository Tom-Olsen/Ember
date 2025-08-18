#include "postRenderEffects.h"



namespace emberEngine
{
	// Constructor/Destructor:
	PostRenderEffects::PostRenderEffects()
	{
		effects.push_back(ComputeShaderManager::GetComputeShader("postProcessing"));
	}
	PostRenderEffects::~PostRenderEffects()
	{
		effects.clear();
	}



	// Overrides:
	void PostRenderEffects::LateUpdate()
	{
		for (ComputeShader* pComputeShader : effects)
			compute::PostRender::RecordComputeShader(pComputeShader);
	}
	const std::string PostRenderEffects::ToString() const
	{
		return "PostRenderEffects";
	}
}