#ifndef __INCLUDE_GUARD_rotationController_h__
#define __INCLUDE_GUARD_rotationController_h__
#include "emberEngine.h"



class RotationController : public Component
{
private: // Members:
	float m_degreesPerSecond;
	float m_translationSpeed;

public: // Methods:
	RotationController(float degreesPerSecond, float translationSpeed = 1.0f);
	~RotationController();

	// Overrides:
	void Update() override;
	const std::string ToString() const override;
};



#endif // __INCLUDE_GUARD_rotationController_h__