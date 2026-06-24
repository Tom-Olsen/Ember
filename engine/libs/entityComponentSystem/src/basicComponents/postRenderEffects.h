#pragma once
#include "component.h"
#include "computeShader.h"
#include <vector>



namespace emberEcs
{
	class PostRenderEffects : public Component
	{
	private: // Members:
		std::vector<emberCore::ComputeShader*> effects;

	public: // Methods:
		PostRenderEffects();
		~PostRenderEffects();

		// Overrides:
		void LateUpdate() override;
	};
}