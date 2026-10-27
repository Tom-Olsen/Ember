#pragma once
#include "component.h"
#include "computeShader.h"
#include "ssaoConstants.h"
#include "texture2DOwner.h"
#include <cstdint>



namespace emberEcs
{
	class ScreenSpaceAmbientOcclusionDispatcher : public Component
	{
	public: // Enums:
		enum class SsaoMode
		{
			off,
			fullResolution,
			halfResolution
		};

	private: // Members:
		emberCore::ComputeShader m_ssaoComputeShader;
		emberCore::ComputeShader m_ssaoMapComputeShader;
		emberCore::ComputeShader m_ssaoUpsampleComputeShader;
		int m_kernelSampleCount = 0;
		uint32_t m_aoWidth = 0;
		uint32_t m_aoHeight = 0;
		emberCore::Texture2DOwner m_ssaoTexture;

	public: // Members:
		SsaoMode ssaoMode = SsaoMode::halfResolution;
		int sampleCount = 32;
		float sampleRadius = 2.0f;
		float sampleBias = 0.01f;
		float occlusionStrength = 3.0f;

	public: // Methods:
		// Constructor/Destructor:
		ScreenSpaceAmbientOcclusionDispatcher();
		~ScreenSpaceAmbientOcclusionDispatcher();

		// Overrides:
		void LateUpdate() override;

	private: // Methods:
		void UpdateSamplingKernel();
		void SetSamplingParameters(emberCore::ComputeShader& computeShader) const;
	};
}