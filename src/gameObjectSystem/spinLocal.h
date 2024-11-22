#pragma once
#ifndef __INCLUDE_GUARD_spin_h__
#define __INCLUDE_GUARD_spin_h__
#include "mathf.h"
#include "component.h"



class SpinLocal : public Component
{
public: // Members:
	Float3 eulerDegreesPerSecond;
	Uint3 rotationOrder;

public: // Methods:
	SpinLocal(Float3 eulerDegreesPerSecond = Float3(), Uint3 rotationOrder = Uint3(1, 0, 2));
	~SpinLocal();

	// Overrides:
	void Update() override;
	std::string ToString() const override;
};



#endif // __INCLUDE_GUARD_spin_h__