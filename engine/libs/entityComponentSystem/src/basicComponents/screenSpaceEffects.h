#pragma once
#include "component.h"
#include "computeShader.h"
#include <vector>



namespace emberEcs
{
	class ScreenSpaceEffects : public Component
	{
	private: // Members:
		std::vector<emberCore::ComputeShader> effects;

	public: // Methods:
		ScreenSpaceEffects();
		~ScreenSpaceEffects();

		// Overrides:
		void LateUpdate() override;
	};
}