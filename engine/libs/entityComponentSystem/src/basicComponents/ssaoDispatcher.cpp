#include "ssaoDispatcher.h"
#include "callProperties.h"
#include "compute.h"
#include "computeShaderManager.h"
#include "emberMath.h"
#include "renderer.h"
#include "ssaoConstants.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <string>
using namespace emberCore;



namespace emberEcs
{
	// Public methods:
	// Constructor/Destructor:
	SsaoDispatcher::SsaoDispatcher()
	{
		m_ssaoKernelSampledComputeShader = ComputeShaderManager::TryGetComputeShader("ssaoKernelSampled");
		m_ssaoBitmaskSampledComputeShader = ComputeShaderManager::TryGetComputeShader("ssaoBitmaskSampled");
		m_ssaoTemporalComputeShader = ComputeShaderManager::TryGetComputeShader("ssaoTemporal");
		m_ssaoUpsampleBlurComputeShader = ComputeShaderManager::TryGetComputeShader("ssaoUpsampleBlur");
		UpdateTextures();
		UpdateSamplingKernel();
	}
	SsaoDispatcher::~SsaoDispatcher() = default;




	// History:
	void SsaoDispatcher::ResetHistory()
	{
		m_historyValid = false;
		m_rngSeed = 0;
	}



	// Overrides:
	void SsaoDispatcher::LateUpdate()
	{
		// No SSAO:
		if (m_ssaoMode == SsaoMode::off)
			return;

		// Prepare gpu resources:
		SetSamplingParameters();
		UpdateTextures();

		// Queue SSAO:
		{
			ComputeShader& ssaoComputeShader = m_ssaoMode == SsaoMode::bitmaskSampled ? m_ssaoBitmaskSampledComputeShader : m_ssaoKernelSampledComputeShader;
			CallProperties callProperties = Compute::PostGeometry::DispatchComputeShader(ssaoComputeShader, Uint3(m_aoWidth, m_aoHeight, 1));
			callProperties.SetTexture("ambientOcclusionMap", m_ssaoTexture.GetTexture());
		}
		Compute::PostGeometry::DispatchBarrierWaitStorageWriteBeforeReadWrite();

		// Queue SSAO temporal:
		{
			CallProperties callProperties = Compute::PostGeometry::DispatchComputeShader(m_ssaoTemporalComputeShader, Uint3(m_aoWidth, m_aoHeight, 1));
			callProperties.SetTexture("rawAmbientOcclusionMap", m_ssaoTexture.GetTexture());
			callProperties.SetTexture("previousAmbientOcclusionMap", m_ambientOcclusionHistoryTextures[1 - m_historyWriteIndex].GetTexture());
			callProperties.SetTexture("previousNormalDepth", m_normalDepthHistoryTextures[1 - m_historyWriteIndex].GetTexture());
			callProperties.SetTexture("currentAmbientOcclusionMap", m_ambientOcclusionHistoryTextures[m_historyWriteIndex].GetTexture());
			callProperties.SetTexture("currentNormalDepth", m_normalDepthHistoryTextures[m_historyWriteIndex].GetTexture());
			callProperties.SetValue("SsaoTemporalParameters", "historyValid", m_historyValid);
			callProperties.SetValue("SsaoTemporalParameters", "historyWeight", m_temporalHistoryWeight);
			callProperties.SetValue("SsaoTemporalParameters", "minimumDepthTolerance", m_temporalMinimumDepthTolerance);
			callProperties.SetValue("SsaoTemporalParameters", "relativeDepthTolerance", m_temporalRelativeDepthTolerance);
			callProperties.SetValue("SsaoTemporalParameters", "minimumNormalSimilarity", m_temporalMinimumNormalSimilarity);
		}
		Compute::PostGeometry::DispatchBarrierWaitStorageWriteBeforeRead();

		// Update history:
		Texture2d currentAmbientOcclusionTexture = m_ambientOcclusionHistoryTextures[m_historyWriteIndex].GetTexture();
		m_historyWriteIndex = 1 - m_historyWriteIndex;
		m_historyValid = true;
		m_rngSeed = (m_rngSeed + 1) % uint32_t(m_temporalRngSeedModulo);

		// Queue temporal reconstruction:
		CallProperties callProperties = Compute::PostGeometry::DispatchComputeShader(m_ssaoUpsampleBlurComputeShader, Uint3(m_renderWidth, m_renderHeight, 1));
		callProperties.SetTexture("ambientOcclusionMap", currentAmbientOcclusionTexture);
		callProperties.SetValue("SsaoReconstructionParameters", "blurRadius", std::min(m_blurRadius, int(std::max(m_aoWidth, m_aoHeight))));
		callProperties.SetValue("SsaoReconstructionParameters", "spatialWeightScale", m_blurSpatialWeightScale);
		callProperties.SetValue("SsaoReconstructionParameters", "minimumDepthTolerance", m_blurMinimumDepthTolerance);
		callProperties.SetValue("SsaoReconstructionParameters", "relativeDepthTolerance", m_blurRelativeDepthTolerance);
		callProperties.SetValue("SsaoReconstructionParameters", "normalWeightExponent", m_blurNormalWeightExponent);
		callProperties.SetValue("SsaoReconstructionParameters", "occlusionStrength", m_occlusionStrength);
	}



	// Setters:
	// Settings:
	void SsaoDispatcher::SetSsaoMode(SsaoMode ssaoMode)
	{
		if (m_ssaoMode != ssaoMode)
		{
			m_ssaoMode = ssaoMode;
			ResetHistory();
		}
	}
	void SsaoDispatcher::SetResolutionScale(float resolutionScale)
	{
		m_resolutionScale = math::Clamp(resolutionScale, 0.1f, 1.0f);
	}
	// Sampling parameters:
	void SsaoDispatcher::SetSampleCount(int sampleCount)
	{
		sampleCount = math::Clamp(sampleCount, 1, SSAO_KERNEL_MAX_SAMPLES);
		if (m_sampleCount == sampleCount)
			return;
		m_sampleCount = sampleCount;
		UpdateSamplingKernel();
		ResetHistory();
	}
	void SsaoDispatcher::SetSampleRadius(float sampleRadius)
	{
		m_sampleRadius = math::Max(0.0001f, sampleRadius);
	}
	void SsaoDispatcher::SetSampleBias(float sampleBias)
	{
		m_sampleBias = math::Max(0.0f, sampleBias);
	}
	void SsaoDispatcher::SetOcclusionStrength(float occlusionStrength)
	{
		m_occlusionStrength = math::Max(0.0f, occlusionStrength);
	}
	// Bitmask sampling parameters:
	void SsaoDispatcher::SetSliceCount(int sliceCount)
	{
		sliceCount = math::Clamp(sliceCount, 1, SSAO_BITMASK_MAX_SLICES);
		if (m_sliceCount == sliceCount)
			return;
		m_sliceCount = sliceCount;
		ResetHistory();
	}
	void SsaoDispatcher::SetSamplesPerSide(int samplesPerSide)
	{
		samplesPerSide = math::Clamp(samplesPerSide, 1, SSAO_BITMASK_MAX_SAMPLES_PER_SIDE);
		if (m_samplesPerSide == samplesPerSide)
			return;
		m_samplesPerSide = samplesPerSide;
		ResetHistory();
	}
	void SsaoDispatcher::SetSampleThickness(float sampleThickness)
	{
		m_sampleThickness = math::Max(0.0f, sampleThickness);
	}
	// Temporal reconstruction parameters:
	void SsaoDispatcher::SetTemporalHistoryWeight(float temporalHistoryWeight)
	{
		m_temporalHistoryWeight = math::Clamp(temporalHistoryWeight, 0.0f, 1.0f);
	}
	void SsaoDispatcher::SetTemporalMinimumDepthTolerance(float temporalMinimumDepthTolerance)
	{
		m_temporalMinimumDepthTolerance = math::Max(0.0001f, temporalMinimumDepthTolerance);
	}
	void SsaoDispatcher::SetTemporalRelativeDepthTolerance(float temporalRelativeDepthTolerance)
	{
		m_temporalRelativeDepthTolerance = math::Max(0.0f, temporalRelativeDepthTolerance);
	}
	void SsaoDispatcher::SetTemporalMinimumNormalSimilarity(float temporalMinimumNormalSimilarity)
	{
		m_temporalMinimumNormalSimilarity = math::Clamp(temporalMinimumNormalSimilarity, 0.0f, 1.0f);
	}
	void SsaoDispatcher::SetTemporalRngSeedModulo(int temporalSeedModulo)
	{
		m_temporalRngSeedModulo = math::Max(1, temporalSeedModulo);
		m_rngSeed %= uint32_t(m_temporalRngSeedModulo);
	}
	// Bilateral blur parameters:
	void SsaoDispatcher::SetBlurRadius(int blurRadius)
	{
		m_blurRadius = math::Max(0, blurRadius);
	}
	void SsaoDispatcher::SetBlurSpatialWeightScale(float blurSpatialWeightScale)
	{
		m_blurSpatialWeightScale = math::Max(0.0f, blurSpatialWeightScale);
	}
	void SsaoDispatcher::SetBlurMinimumDepthTolerance(float blurMinimumDepthTolerance)
	{
		m_blurMinimumDepthTolerance = math::Max(0.0001f, blurMinimumDepthTolerance);
	}
	void SsaoDispatcher::SetBlurRelativeDepthTolerance(float blurRelativeDepthTolerance)
	{
		m_blurRelativeDepthTolerance = math::Max(0.0f, blurRelativeDepthTolerance);
	}
	void SsaoDispatcher::SetBlurNormalWeightExponent(float blurNormalWeightExponent)
	{
		m_blurNormalWeightExponent = math::Max(0.0f, blurNormalWeightExponent);
	}



	// Getters:
	// Settings:
	SsaoDispatcher::SsaoMode SsaoDispatcher::GetSsaoMode() const
	{
		return m_ssaoMode;
	}
	float SsaoDispatcher::GetResolutionScale() const
	{
		return m_resolutionScale;
	}
	// Sampling parameters:
	int SsaoDispatcher::GetSampleCount() const
	{
		return m_sampleCount;
	}
	float SsaoDispatcher::GetSampleRadius() const
	{
		return m_sampleRadius;
	}
	float SsaoDispatcher::GetSampleBias() const
	{
		return m_sampleBias;
	}
	float SsaoDispatcher::GetOcclusionStrength() const
	{
		return m_occlusionStrength;
	}
	// Bitmask sampling parameters:
	int SsaoDispatcher::GetSliceCount() const
	{
		return m_sliceCount;
	}
	int SsaoDispatcher::GetSamplesPerSide() const
	{
		return m_samplesPerSide;
	}
	float SsaoDispatcher::GetSampleThickness() const
	{
		return m_sampleThickness;
	}
	// Temporal reconstruction parameters:
	float SsaoDispatcher::GetTemporalHistoryWeight() const
	{
		return m_temporalHistoryWeight;
	}
	float SsaoDispatcher::GetTemporalMinimumDepthTolerance() const
	{
		return m_temporalMinimumDepthTolerance;
	}
	float SsaoDispatcher::GetTemporalRelativeDepthTolerance() const
	{
		return m_temporalRelativeDepthTolerance;
	}
	float SsaoDispatcher::GetTemporalMinimumNormalSimilarity() const
	{
		return m_temporalMinimumNormalSimilarity;
	}
	int SsaoDispatcher::GetTemporalRngSeedModulo() const
	{
		return m_temporalRngSeedModulo;
	}
	// Bilateral blur parameters:
	int SsaoDispatcher::GetBlurRadius() const
	{
		return m_blurRadius;
	}
	float SsaoDispatcher::GetBlurSpatialWeightScale() const
	{
		return m_blurSpatialWeightScale;
	}
	float SsaoDispatcher::GetBlurMinimumDepthTolerance() const
	{
		return m_blurMinimumDepthTolerance;
	}
	float SsaoDispatcher::GetBlurRelativeDepthTolerance() const
	{
		return m_blurRelativeDepthTolerance;
	}
	float SsaoDispatcher::GetBlurNormalWeightExponent() const
	{
		return m_blurNormalWeightExponent;
	}



	// Private methods:
	// Helpers:
	void SsaoDispatcher::UpdateTextures()
	{
		// New resolutions:
		uint32_t renderWidth = Renderer::GetRenderWidth();
		uint32_t renderHeight = Renderer::GetRenderHeight();
		uint32_t aoWidth = math::Max(1u, uint32_t(math::Ceil(double(renderWidth) * m_resolutionScale)));
		uint32_t aoHeight = math::Max(1u, uint32_t(math::Ceil(double(renderHeight) * m_resolutionScale)));
		
		// Update render resolution:
		if (renderWidth != m_renderWidth || renderHeight != m_renderHeight)
			ResetHistory();
		m_renderWidth = renderWidth;
		m_renderHeight = renderHeight;
		
		// Update AO map resolution:
		if (aoWidth == m_aoWidth && aoHeight == m_aoHeight)
			return;
		m_aoWidth = aoWidth;
		m_aoHeight = aoHeight;

		// AO map resolution changed, reallocate textures:
		m_ssaoTexture = Texture2DOwner("ssaoTexture", m_aoWidth, m_aoHeight, emberCommon::TextureFormats::r16_sfloat, emberCommon::TextureUsage::storage, emberCommon::TextureImageCountMode::perFrameInFlight);
		for (uint32_t i = 0; i < m_ambientOcclusionHistoryTextures.size(); i++)
		{
			m_ambientOcclusionHistoryTextures[i] = Texture2DOwner("ssaoAmbientOcclusionHistory" + std::to_string(i), m_aoWidth, m_aoHeight, emberCommon::TextureFormats::r16_sfloat, emberCommon::TextureUsage::storage);
			m_normalDepthHistoryTextures[i] = Texture2DOwner("ssaoNormalDepthHistory" + std::to_string(i), m_aoWidth, m_aoHeight, emberCommon::TextureFormats::rgba32_sfloat, emberCommon::TextureUsage::storage);
		}
		ResetHistory();
	}
	void SsaoDispatcher::UpdateSamplingKernel()
	{
		// Fibonacci spiral on a cosine weighted hemisphere around local +z:
		for (int i = 0; i < m_sampleCount; i++)
		{
			float directionFraction = (float(i) + 0.5f) / float(m_sampleCount);
			Radians angle = Radians(math::goldenAngle * float(i));
			float diskRadius = math::Sqrt(directionFraction);
			Float3 direction(diskRadius * math::Cos(angle), diskRadius * math::Sin(angle), math::Sqrt(1.0f - directionFraction));
			m_ssaoKernelSampledComputeShader.SetValue("SamplingKernel", "sampleDirections", uint32_t(i), direction);
		}
	}
	void SsaoDispatcher::SetSamplingParameters()
	{
		if (m_ssaoMode == SsaoMode::kernelSampled)
		{
			m_ssaoKernelSampledComputeShader.SetValue("SsaoSamplingParameters", "sampleCount", m_sampleCount);
			m_ssaoKernelSampledComputeShader.SetValue("SsaoSamplingParameters", "sampleRadius", m_sampleRadius);
			m_ssaoKernelSampledComputeShader.SetValue("SsaoSamplingParameters", "sampleBias", m_sampleBias);
			m_ssaoKernelSampledComputeShader.SetValue("SsaoSamplingParameters", "rngSeed", int(m_rngSeed));
		}
		else if (m_ssaoMode == SsaoMode::bitmaskSampled)
		{
			m_ssaoBitmaskSampledComputeShader.SetValue("SsaoSamplingParameters", "sliceCount", m_sliceCount);
			m_ssaoBitmaskSampledComputeShader.SetValue("SsaoSamplingParameters", "samplesPerSide", m_samplesPerSide);
			m_ssaoBitmaskSampledComputeShader.SetValue("SsaoSamplingParameters", "sampleRadius", m_sampleRadius);
			m_ssaoBitmaskSampledComputeShader.SetValue("SsaoSamplingParameters", "sampleBias", m_sampleBias);
			m_ssaoBitmaskSampledComputeShader.SetValue("SsaoSamplingParameters", "sampleThickness", m_sampleThickness);
			m_ssaoBitmaskSampledComputeShader.SetValue("SsaoSamplingParameters", "rngSeed", int(m_rngSeed));
		}
	}
}