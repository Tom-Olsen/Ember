#ifndef __INCLUDE_GUARD_spinLocal_h__
#define __INCLUDE_GUARD_spinLocal_h__
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
		const std::string ToString() const override;
	};
}



#endif // __INCLUDE_GUARD_spinLocal_h__