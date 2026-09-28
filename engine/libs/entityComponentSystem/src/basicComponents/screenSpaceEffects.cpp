#include "screenSpaceEffects.h"
#include "callProperties.h"
#include "compute.h"
#include "computeShaderManager.h"
using namespace emberCore;



namespace emberEcs
{
	// Constructor/Destructor:
	ScreenSpaceEffects::ScreenSpaceEffects(Texture& environmentMap)
		: m_pEnvironmentMap(&environmentMap)
	{
		m_effects.push_back(ComputeShaderManager::TryGetComputeShader("screenSpaceReflections"));
	}
	ScreenSpaceEffects::~ScreenSpaceEffects()
	{
		m_effects.clear();
	}



	// Overrides:
	void ScreenSpaceEffects::LateUpdate()
	{
		for (ComputeShader& computeShader : m_effects)
		{
			CallProperties callProperties = Compute::ScreenSpace::RecordComputeShader(computeShader);
			callProperties.SetTexture("environmentMap", *m_pEnvironmentMap);
		}
	}
}