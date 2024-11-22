#pragma once
#ifndef __INCLUDE_GUARD_SpinAroundOrigin_h__
#define __INCLUDE_GUARD_SpinAroundOrigin_h__
#include "mathf.h"
#include "component.h"



class SpinGlobal : public Component
{
public: // Members:
	Float3 position;
	Float3 eulerDegreesPerSecond;
	Uint3 rotationOrder;

public: // Methods:
	SpinGlobal(Float3 position, Float3 eulerDegreesPerSecond = Float3(), Uint3 rotationOrder = Uint3(1, 0, 2));
	~SpinGlobal();

	// Overrides:
	void Update() override;
	std::string ToString() const override;
};



#endif // __INCLUDE_GUARD_SpinAroundOrigin_h__