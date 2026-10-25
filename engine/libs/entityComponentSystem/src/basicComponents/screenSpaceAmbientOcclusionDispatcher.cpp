#include "screenSpaceAmbientOcclusionDispatcher.h"
#include "callProperties.h"
#include "compute.h"
#include "computeShaderManager.h"
#include "renderer.h"
using namespace emberCore;



namespace emberEcs
{
	// Constructor/Destructor:
	ScreenSpaceAmbientOcclusionDispatcher::ScreenSpaceAmbientOcclusionDispatcher()
	{
		m_ssaoComputeShader = ComputeShaderManager::TryGetComputeShader("screenSpaceAmbientOcclusion");
	}
	ScreenSpaceAmbientOcclusionDispatcher::~ScreenSpaceAmbientOcclusionDispatcher()
	{
		m_ssaoComputeShader = emberCore::ComputeShader();
	}



	// Overrides:
	void ScreenSpaceAmbientOcclusionDispatcher::LateUpdate()
	{
		Compute::PostGeometry::RecordComputeShader(m_ssaoComputeShader, Uint3(Renderer::GetRenderWidth(), Renderer::GetRenderHeight(), 1));
	}
}