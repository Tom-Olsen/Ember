#pragma once
#include "emberEngine.h"



namespace emberEcs
{
	class PostRenderEffects : public Component
	{
	private: // Members:
		std::vector<ComputeShader*> effects;

	public: // Methods:
		PostRenderEffects();
		~PostRenderEffects();

		// Overrides:
		void LateUpdate() override;
	};
}