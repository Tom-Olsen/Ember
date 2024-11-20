#pragma once
#ifndef __INCLUDE_GUARD_SpinAroundOrigin_h__
#define __INCLUDE_GUARD_SpinAroundOrigin_h__
#include "mathf.h"
#include "component.h"



class SpinAroundOrigin : public Component
{
public: // Members:
	float degreesPerSecond;

public: // Methods:
	SpinAroundOrigin(float degreesPerSecond = 0.0f);
	~SpinAroundOrigin();

	// Overrides:
	void Update() override;
	std::string ToString() const override;
};



#endif // __INCLUDE_GUARD_SpinAroundOrigin_h__