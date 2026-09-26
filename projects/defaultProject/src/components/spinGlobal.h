#pragma once
#include "emberEngine.h"



namespace emberEngine
{
	class SpinGlobal : public emberEcs::Component
	{
	private: // Members:
		Float3 m_position;
		EulerDegrees m_speed;
		Uint3 m_rotationOrder;
		bool m_spin = true;

	public: // Methods:
		SpinGlobal(Float3 position, EulerDegrees speed = EulerDegrees(), Uint3 rotationOrder = Uint3(1, 0, 2));
		~SpinGlobal();

		// Overrides:
		void Update() override;
	};
}