#pragma once
#include "component.h"
#include "computeShader.h"
#include "texture2d.h"



namespace emberEcs
{
	class ScreenSpaceReflectionDispatcher : public Component
	{
	private: // Members:
		emberCore::ComputeShader m_ssrComputeShader;				// sceneColorA -> sceneColorB direct reflections
		emberCore::ComputeShader m_ssrReflectionMapComputeShader;	// sceneColorA (read) -> reflectionTexture
		emberCore::ComputeShader m_ssrCompositComputeShader;		// reflectionTexture -> sceneColorA (write)
		emberCore::ComputeShader m_ssrReflectionUvMapComputeShader;
		emberCore::ComputeShader m_ssrUvCompositComputeShader;
		int m_reflectionWidth;
		int m_reflectionHeight;
		emberCore::Texture2d m_ssrReflectionTexture;
		emberCore::Texture2d m_ssrReflectionUvTexture;
		emberCore::Texture* m_pEnvironmentMap;

	public: // Members:
		int ssrMode;

	public: // Methods:
		ScreenSpaceReflectionDispatcher(emberCore::Texture& environmentMap);
		~ScreenSpaceReflectionDispatcher();

		// Overrides:
		void LateUpdate() override;
	};
}