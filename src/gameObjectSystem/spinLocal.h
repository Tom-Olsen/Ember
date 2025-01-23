#ifndef __INCLUDE_GUARD_spin_h__
#define __INCLUDE_GUARD_spin_h__
#include "emberEngine.h"



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



#endif // __INCLUDE_GUARD_spin_h__