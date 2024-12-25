#ifndef __INCLUDE_GUARD_rotationController_h__
#define __INCLUDE_GUARD_rotationController_h__
#include "emberEngine.h"



class RotationController : public Component
{
private: // Members:
	float m_degreesPerSecond;


public: // Methods:
	RotationController(float degreesPerSecond);
	~RotationController();

	// Overrides:
	void Update() override;
	const std::string ToString() const override;
};



#endif // __INCLUDE_GUARD_rotationController_h__