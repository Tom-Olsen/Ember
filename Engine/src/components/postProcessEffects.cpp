#include "postProcessEffects.h"



namespace emberEngine
{
	// Constructor/Destructor:
	PostProcessEffects::PostProcessEffects()
	{
		effects.push_back(ComputeShaderManager::GetComputeShader("postProcessing"));
	}
	PostProcessEffects::~PostProcessEffects()
	{
		effects.clear();
	}



	// Overrides:
	void PostProcessEffects::LateUpdate()
	{
		for (ComputeShader* pComputeShader : effects)
			Graphics::PostProcess(pComputeShader);
	}
	const std::string PostProcessEffects::ToString() const
	{
		return "PostProcessEffects";
	}
}