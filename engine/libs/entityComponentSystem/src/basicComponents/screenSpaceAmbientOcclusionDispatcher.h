#pragma once
#include "component.h"
#include "computeShader.h"



namespace emberEcs
{
	class ScreenSpaceAmbientOcclusionDispatcher : public Component
	{
	private: // Members:
		emberCore::ComputeShader m_ssaoComputeShader;

	public: // Methods:
		// Constructor/Destructor:
		ScreenSpaceAmbientOcclusionDispatcher();
		~ScreenSpaceAmbientOcclusionDispatcher();

		// Overrides:
		void LateUpdate() override;
	};
}