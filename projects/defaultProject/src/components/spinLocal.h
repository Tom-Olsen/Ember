#pragma once
#include "emberEngine.h"



namespace emberEngine
{
	class SpinLocal : public Component
	{
	private: // Members:
		float m_speed;

	public: // Methods:
		SpinLocal(float speed = 45.0f);
		~SpinLocal();
		void SetSpeed(float speed);
		float GetSpeed() const;

		// Overrides:
		void Update() override;
	};
}