#include "screenSpaceAmbientOcclusionDispatcher.h"
#include "callProperties.h"
#include "compute.h"
#include "computeShaderManager.h"
#include "emberMath.h"
#include "renderer.h"
using namespace emberCore;



namespace emberEcs
{
	// Public methods:
	// Constructor/Destructor:
	ScreenSpaceAmbientOcclusionDispatcher::ScreenSpaceAmbientOcclusionDispatcher()
	{
		m_ssaoComputeShader = ComputeShaderManager::TryGetComputeShader("screenSpaceAmbientOcclusion");
		m_ssaoMapComputeShader = ComputeShaderManager::TryGetComputeShader("screenSpaceAmbientOcclusionMap");
		m_ssaoUpsampleComputeShader = ComputeShaderManager::TryGetComputeShader("screenSpaceAmbientOcclusionUpsample");
		m_aoWidth = emberCore::Renderer::GetRenderWidth() / 2;
		m_aoHeight = emberCore::Renderer::GetRenderHeight() / 2;
		m_ssaoTexture = Texture2DOwner("ssaoTexture", m_aoWidth, m_aoHeight, emberCommon::TextureFormats::r16_sfloat, emberCommon::TextureUsage::storage, emberCommon::TextureImageCountMode::perFrameInFlight);
	}
	ScreenSpaceAmbientOcclusionDispatcher::~ScreenSpaceAmbientOcclusionDispatcher()
	{
		m_ssaoComputeShader = emberCore::ComputeShader();
		m_ssaoMapComputeShader = emberCore::ComputeShader();
		m_ssaoUpsampleComputeShader = emberCore::ComputeShader();
	}



	// Overrides:
	void ScreenSpaceAmbientOcclusionDispatcher::LateUpdate()
	{
		if (ssaoMode == SsaoMode::off)
			return;
		UpdateSamplingKernel();
		if (ssaoMode == SsaoMode::fullResolution)
		{
			SetSamplingParameters(m_ssaoComputeShader);
			Compute::PostGeometry::RecordComputeShader(m_ssaoComputeShader, Uint3(emberCore::Renderer::GetRenderWidth(), emberCore::Renderer::GetRenderHeight(), 1));
		}
		else if (ssaoMode == SsaoMode::halfResolution)
		{
			SetSamplingParameters(m_ssaoMapComputeShader);
			SetSamplingParameters(m_ssaoUpsampleComputeShader);

			// Low resolution occlusion:
			{
				CallProperties callProperties = Compute::PostGeometry::RecordComputeShader(m_ssaoMapComputeShader, Uint3(m_aoWidth, m_aoHeight, 1));
				callProperties.SetTexture("ambientOcclusionMap", m_ssaoTexture.GetTexture());
			}
			// Rinish write before read barrier:
			Compute::PostGeometry::RecordBarrierWaitStorageWriteBeforeRead();
			// Upsample into G-buffer:
			{
				CallProperties callProperties = Compute::PostGeometry::RecordComputeShader(m_ssaoUpsampleComputeShader, Uint3(emberCore::Renderer::GetRenderWidth(), emberCore::Renderer::GetRenderHeight(), 1));
				callProperties.SetTexture("ambientOcclusionMap", m_ssaoTexture.GetTexture());
			}
		}
	}



	// Private methods:
	void ScreenSpaceAmbientOcclusionDispatcher::UpdateSamplingKernel()
	{
		sampleCount = math::Clamp(sampleCount, 1, SSAO_MAX_SAMPLE_COUNT);
		if (m_kernelSampleCount == sampleCount)
			return;

		// Fibonacci spiral on a cosine weighted hemisphere around local +Z:
		for (int i = 0; i < sampleCount; i++)
		{
			float directionFraction = (float(i) + 0.5f) / float(sampleCount);
			Radians angle = Radians(math::goldenAngle * float(i));
			float diskRadius = math::Sqrt(directionFraction);
			Float3 direction(diskRadius * math::Cos(angle), diskRadius * math::Sin(angle), math::Sqrt(1.0f - directionFraction));
			m_ssaoComputeShader.SetValue("SsaoSamplingKernel", "sampleDirections", uint32_t(i), direction);
			m_ssaoMapComputeShader.SetValue("SsaoSamplingKernel", "sampleDirections", uint32_t(i), direction);
		}
		m_kernelSampleCount = sampleCount;
	}
	void ScreenSpaceAmbientOcclusionDispatcher::SetSamplingParameters(ComputeShader& computeShader) const
	{
		computeShader.SetValue("SsaoSamplingParameters", "sampleCount", sampleCount);
		computeShader.SetValue("SsaoSamplingParameters", "sampleRadius", sampleRadius);
		computeShader.SetValue("SsaoSamplingParameters", "sampleBias", sampleBias);
		computeShader.SetValue("SsaoSamplingParameters", "occlusionStrength", occlusionStrength);
	}
}