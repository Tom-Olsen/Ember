#include "screenSpaceEffects.h"
#include "callProperties.h"
#include "compute.h"
#include "computeShaderManager.h"
using namespace emberCore;



namespace emberEcs
{
	// Constructor/Destructor:
	ScreenSpaceEffects::ScreenSpaceEffects()
	{
		effects.push_back(ComputeShaderManager::TryGetComputeShader("screenSpaceReflections"));
	}
	ScreenSpaceEffects::~ScreenSpaceEffects()
	{
		effects.clear();
	}



	// Overrides:
	void ScreenSpaceEffects::LateUpdate()
	{
		for (ComputeShader& computeShader : effects)
			CallProperties callProperties = Compute::ScreenSpace::RecordComputeShader(computeShader);
	}
}