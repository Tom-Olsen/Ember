#pragma once
#include "emberEngine.h"



namespace emberEngine
{
	class SpinLocal : public emberEcs::Component
	{
	private: // Members:
		Degrees m_speed;

	public: // Methods:
		SpinLocal(Degrees speed = Degrees(45.0f));
		~SpinLocal();
		void SetSpeed(Degrees speed);
		Degrees GetSpeed() const;

		// Overrides:
		void Update() override;
	};
}