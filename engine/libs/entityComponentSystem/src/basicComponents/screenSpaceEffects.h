#pragma once
#include "component.h"
#include "computeShader.h"
#include <vector>



namespace emberCore
{
	class Texture;
}



namespace emberEcs
{
	class ScreenSpaceEffects : public Component
	{
	private: // Members:
		std::vector<emberCore::ComputeShader> effects;
		emberCore::Texture* m_pEnvironmentMap;

	public: // Methods:
		ScreenSpaceEffects(emberCore::Texture& environmentMap);
		~ScreenSpaceEffects();

		// Overrides:
		void LateUpdate() override;
	};
}