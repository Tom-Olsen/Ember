#include "spinLocal.h"



// Constructor/Destructor:
SpinLocal::SpinLocal(Float3 eulerDegreesPerSecond, Uint3 rotationOrder)
{
	m_eulerDegreesPerSecond = eulerDegreesPerSecond;
	m_rotationOrder = rotationOrder;
}
SpinLocal::~SpinLocal()
{

}



// Overrides:
void SpinLocal::Update()
{
	if (EventSystem::KeyDownOrHeld(SDLK_LEFT))
	{
		Float3 eulerRadians = -m_eulerDegreesPerSecond.ToRadians() * Timer::GetDeltaTime();
		Float3x3 rotation = Float3x3::Rotate(eulerRadians, m_rotationOrder);
		GetTransform()->SetRotationMatrix(GetTransform()->GetRotation3x3() * rotation);
	}
	if (EventSystem::KeyDownOrHeld(SDLK_RIGHT))
	{
		Float3 eulerRadians = m_eulerDegreesPerSecond.ToRadians() * Timer::GetDeltaTime();
		Float3x3 rotation = Float3x3::Rotate(eulerRadians, m_rotationOrder);
		GetTransform()->SetRotationMatrix(GetTransform()->GetRotation3x3() * rotation);
	}
}
const std::string SpinLocal::ToString() const
{
	return "SpinLocal";
}