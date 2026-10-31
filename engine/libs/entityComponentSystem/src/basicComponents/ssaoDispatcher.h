#pragma once
#include "component.h"
#include "computeShader.h"
#include "texture2DOwner.h"
#include <array>
#include <cstdint>



namespace emberEcs
{
	class SsaoDispatcher : public Component
	{
	private: // Members:
		// Settings:
		bool m_ssaoEnabled = true;
		float m_resolutionScale = 0.5f;
		// Sampling parameters:
		int m_sampleCount = 32;
		float m_sampleRadius = 2.0f;
		float m_sampleBias = 0.01f;
		float m_occlusionStrength = 3.0f;
		// Temporal reconstruction parameters:
		float m_temporalHistoryWeight = 0.9f;
		float m_temporalMinimumDepthTolerance = 0.05f;
		float m_temporalRelativeDepthTolerance = 0.02f;
		float m_temporalMinimumNormalSimilarity = 0.9f;
		int m_temporalRngSeedModulo = 8;
		// Bilateral blur parameters:
		int m_blurRadius = 1;	// in ao texels.
		float m_blurSpatialWeightScale = 0.0f;
		float m_blurMinimumDepthTolerance = 0.05f;
		float m_blurRelativeDepthTolerance = 0.02f;
		float m_blurNormalWeightExponent = 32.0f;
		// Render/sample resolution (internal):
		uint32_t m_renderWidth = 0;
		uint32_t m_renderHeight = 0;
		uint32_t m_aoWidth = 0;
		uint32_t m_aoHeight = 0;
		// History (internal):
		bool m_historyValid = false;
		uint32_t m_historyWriteIndex = 0;
		uint32_t m_rngSeed = 0;
		// Compute shaders (internal):
		emberCore::ComputeShader m_ssaoComputeShader;
		emberCore::ComputeShader m_ssaoTemporalComputeShader;
		emberCore::ComputeShader m_ssaoUpsampleBlurComputeShader;
		// Textures (internal):
		emberCore::Texture2DOwner m_ssaoTexture;
		std::array<emberCore::Texture2DOwner, 2> m_ambientOcclusionHistoryTextures;
		std::array<emberCore::Texture2DOwner, 2> m_normalDepthHistoryTextures;

	public: // Methods:
		// Constructor/Destructor:
		SsaoDispatcher();
		~SsaoDispatcher();

		// History:
		void ResetHistory();

		// Overrides:
		void LateUpdate() override;

		// Setters:
		// Settings:
		void SetSsaoEnabled(bool ssaoEnabled);
		void SetResolutionScale(float resolutionScale);
		// Sampling parameters:
		void SetSampleCount(int sampleCount);
		void SetSampleRadius(float sampleRadius);
		void SetSampleBias(float sampleBias);
		void SetOcclusionStrength(float occlusionStrength);
		// Temporal reconstruction parameters:
		void SetTemporalHistoryWeight(float temporalHistoryWeight);
		void SetTemporalMinimumDepthTolerance(float temporalMinimumDepthTolerance);
		void SetTemporalRelativeDepthTolerance(float temporalRelativeDepthTolerance);
		void SetTemporalMinimumNormalSimilarity(float temporalMinimumNormalSimilarity);
		void SetTemporalRngSeedModulo(int temporalSeedModulo);
		// Bilateral blur parameters:
		void SetBlurRadius(int blurRadius);
		void SetBlurSpatialWeightScale(float blurSpatialWeightScale);
		void SetBlurMinimumDepthTolerance(float blurMinimumDepthTolerance);
		void SetBlurRelativeDepthTolerance(float blurRelativeDepthTolerance);
		void SetBlurNormalWeightExponent(float blurNormalWeightExponent);

		// Getters:
		// Settings:
		bool GetSsaoEnabled() const;
		float GetResolutionScale() const;
		// Sampling parameters:
		int GetSampleCount() const;
		float GetSampleRadius() const;
		float GetSampleBias() const;
		float GetOcclusionStrength() const;
		// Temporal reconstruction parameters:
		float GetTemporalHistoryWeight() const;
		float GetTemporalMinimumDepthTolerance() const;
		float GetTemporalRelativeDepthTolerance() const;
		float GetTemporalMinimumNormalSimilarity() const;
		int GetTemporalRngSeedModulo() const;
		// Bilateral blur parameters:
		int GetBlurRadius() const;
		float GetBlurSpatialWeightScale() const;
		float GetBlurMinimumDepthTolerance() const;
		float GetBlurRelativeDepthTolerance() const;
		float GetBlurNormalWeightExponent() const;

	private: // Methods:
		// Helpers:
		void UpdateTextures();
		void UpdateSamplingKernel();
		void SetSamplingParameters();
	};
}