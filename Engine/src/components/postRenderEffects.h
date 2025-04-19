#ifndef __INCLUDE_GUARD_postRenderEffects_h__
#define __INCLUDE_GUARD_postRenderEffects_h__
#include "emberEngine.h"



namespace emberEngine
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
		const std::string ToString() const override;
	};
}



#endif // __INCLUDE_GUARD_postRenderEffects_h__