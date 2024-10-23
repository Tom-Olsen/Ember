#pragma once
#ifndef __INCLUDE_GUARD_spin_h__
#define __INCLUDE_GUARD_spin_h__
#include "glmTypes.h"
#include "component.h"



class Spin : public Component
{
public: // Members:
	Float3 eulerAnglesPerSecond;

public: // Methods:
	Spin(Float3 eulerAnglesPerSecond = Float3());
	~Spin();

	// Overrides:
	void Update() override;
	std::string ToString() const override;
};



#endif // __INCLUDE_GUARD_spin_h__