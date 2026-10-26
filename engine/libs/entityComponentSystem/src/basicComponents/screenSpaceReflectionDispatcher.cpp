#include "screenSpaceReflectionDispatcher.h"
#include "callProperties.h"
#include "compute.h"
#include "computeShaderManager.h"
#include "renderer.h"
using namespace emberCore;



namespace emberEcs
{
	// Constructor/Destructor:
	ScreenSpaceReflectionDispatcher::ScreenSpaceReflectionDispatcher(Texture environmentMap)
		: m_environmentMap(environmentMap)
	{
		ssrMode = SsrMode::reflectionUVMap;
		m_ssrComputeShader = ComputeShaderManager::TryGetComputeShader("screenSpaceReflections");
		m_ssrReflectionMapComputeShader = ComputeShaderManager::TryGetComputeShader("screenSpaceReflectionMap");
		m_ssrCompositComputeShader = ComputeShaderManager::TryGetComputeShader("screenSpaceReflectionComposit");
		m_ssrReflectionUvMapComputeShader = ComputeShaderManager::TryGetComputeShader("screenSpaceReflectionUvMap");
		m_ssrUvCompositComputeShader = ComputeShaderManager::TryGetComputeShader("screenSpaceReflectionUvComposit");
		m_reflectionWidth = emberCore::Renderer::GetRenderWidth() / 2;
		m_reflectionHeight = emberCore::Renderer::GetRenderHeight() / 2;
		m_ssrReflectionTexture = emberCore::Texture2DOwner("ssrReflectionTexture", m_reflectionWidth, m_reflectionHeight, emberCommon::TextureFormats::rgba16_sfloat, emberCommon::TextureUsage::storage, emberCommon::TextureImageCountMode::perFrameInFlight);
		m_ssrReflectionUvTexture = emberCore::Texture2DOwner("ssrReflectionUvTexture", m_reflectionWidth, m_reflectionHeight, emberCommon::TextureFormats::rgba16_unorm, emberCommon::TextureUsage::storage, emberCommon::TextureImageCountMode::perFrameInFlight);
	}
	ScreenSpaceReflectionDispatcher::~ScreenSpaceReflectionDispatcher()
	{
		m_ssrComputeShader = emberCore::ComputeShader();
		m_ssrReflectionMapComputeShader = emberCore::ComputeShader();
		m_ssrCompositComputeShader = emberCore::ComputeShader();
		m_ssrReflectionUvMapComputeShader = emberCore::ComputeShader();
		m_ssrUvCompositComputeShader = emberCore::ComputeShader();
	}



	// Overrides:
	void ScreenSpaceReflectionDispatcher::LateUpdate()
	{
		// sceneColorA -> sceneColorB direct reflections
		if (ssrMode == SsrMode::off)
			return;
		if (ssrMode == SsrMode::fullResolution)
		{
			CallProperties callProperties = Compute::PostLighting::RecordComputeShader(m_ssrComputeShader, Uint3(emberCore::Renderer::GetRenderWidth(), emberCore::Renderer::GetRenderHeight(), 1));
			callProperties.SetTexture("environmentMap", m_environmentMap);
		}
		else if (ssrMode == SsrMode::reflectionMap)
		{
			// sceneColorA (read) -> reflectionTexture
			{
				CallProperties callProperties = Compute::PostLighting::RecordComputeShader(m_ssrReflectionMapComputeShader, Uint3(m_reflectionWidth, m_reflectionHeight, 1));
				callProperties.SetTexture("environmentMap", m_environmentMap);
				callProperties.SetTexture("reflectionMap", m_ssrReflectionTexture.GetTexture());
			}
			// finish write before read barrier
			Compute::PostLighting::RecordBarrierWaitStorageWriteBeforeRead();
			// reflectionTexture -> sceneColorA (write)
			{
				CallProperties callProperties = Compute::PostLighting::RecordComputeShader(m_ssrCompositComputeShader, Uint3(emberCore::Renderer::GetRenderWidth(), emberCore::Renderer::GetRenderHeight(), 1));
				callProperties.SetTexture("reflectionMap", m_ssrReflectionTexture.GetTexture());
			}
		}
		else if (ssrMode == SsrMode::reflectionUVMap)
		{
			// sceneColorA metadata -> reflectionUvTexture
			{
				CallProperties callProperties = Compute::PostLighting::RecordComputeShader(m_ssrReflectionUvMapComputeShader, Uint3(m_reflectionWidth, m_reflectionHeight, 1));
				callProperties.SetTexture("reflectionUvMap", m_ssrReflectionUvTexture.GetTexture());
			}
			// finish write before read barrier
			Compute::PostLighting::RecordBarrierWaitStorageWriteBeforeRead();
			// reflectionUvTexture + sceneColorA (read) -> sceneColorB (write)
			{
				CallProperties callProperties = Compute::PostLighting::RecordComputeShader(m_ssrUvCompositComputeShader, Uint3(emberCore::Renderer::GetRenderWidth(), emberCore::Renderer::GetRenderHeight(), 1));
				callProperties.SetTexture("environmentMap", m_environmentMap);
				callProperties.SetTexture("reflectionUvMap", m_ssrReflectionUvTexture.GetTexture());
			}
		}
	}
}