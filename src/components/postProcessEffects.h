#ifndef __INCLUDE_GUARD_postProcessEffects_h__
#define __INCLUDE_GUARD_postProcessEffects_h__
#include "emberEngine.h"



namespace emberEngine
{
	class PostProcessEffects : public Component
	{
	private: // Members:
		std::vector<ComputeShader*> effects;

	public: // Methods:
		PostProcessEffects();
		~PostProcessEffects();

		// Overrides:
		void LateUpdate() override;
		const std::string ToString() const override;
	};
}



#endif // __INCLUDE_GUARD_postProcessEffects_h__