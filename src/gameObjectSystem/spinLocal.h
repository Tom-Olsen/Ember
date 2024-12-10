#ifndef __INCLUDE_GUARD_spin_h__
#define __INCLUDE_GUARD_spin_h__
#include "emberEngine.h"



class SpinLocal : public Component
{
private: // Members:
	Float3 m_eulerDegreesPerSecond;
	Uint3 m_rotationOrder;

public: // Methods:
	SpinLocal(Float3 eulerDegreesPerSecond = Float3(), Uint3 rotationOrder = Uint3(1, 0, 2));
	~SpinLocal();

	// Overrides:
	void Update() override;
	const std::string ToString() const override;
};



#endif // __INCLUDE_GUARD_spin_h__