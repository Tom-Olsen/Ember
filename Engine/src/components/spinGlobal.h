#ifndef __INCLUDE_GUARD_SpinAroundOrigin_h__
#define __INCLUDE_GUARD_SpinAroundOrigin_h__
#include "emberEngine.h"



namespace emberEngine
{
	class SpinGlobal : public Component
	{
	private: // Members:
		Float3 m_position;
		Float3 m_eulerDegreesPerSecond;
		Uint3 m_rotationOrder;
		bool m_spin = true;

	public: // Methods:
		SpinGlobal(Float3 position, Float3 eulerDegreesPerSecond = Float3(), Uint3 rotationOrder = Uint3(1, 0, 2));
		~SpinGlobal();

		// Overrides:
		void Update() override;
		const std::string ToString() const override;
	};
}



#endif // __INCLUDE_GUARD_SpinAroundOrigin_h__